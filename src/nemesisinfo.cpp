#include "Global.h"
#include "debuglog.h"
#include "nemesisinfo.h"

#include <Windows.h>

#include <QtCore/QFile.h>
#include <QtCore/QTextStream.h>

#include "utilities/regex.h"
#include "utilities/writetextfile.h"

using namespace std;
namespace sf = filesystem;

bool SSE = false;
wstring stagePath = L"";
sf::path CurrentExePath;
sf::path CurrentExeDirectory;

NemesisInfo* NemesisInfo::nemesisinfo;
std::atomic_flag NemesisInfo::lock{};

void NemesisInfo::setup()
{
    ReadNemesisInfoFile();
    sf::path curpath = sf::current_path();
    curpath.make_preferred();

    if (nemesis::isearch(curpath.wstring(), L"\\project new reign - nemesis\\test environment") != NOT_FOUND)
    {
        dataPath = curpath / L"data\\";
        curpath /= L"Nemesis_Engine";
    }
    else
    {
        PathValidation();
    }

    SetDataPath();

    wstring nmpath = dataPath / L"Nemesis_Engine";

    if (!forceDirectory && nemesis::iequals(nmpath, curpath)) ErrorMessage(6010, curpath, nmpath);

    stageDirectory = !stagePath.empty() ? (stagePath + L"\\") : dataPath;
    iniFileUpdate();
}

NemesisInfo::NemesisInfo()
{
    try
    {
        setup();
    }
    catch (nemesis::exception&)
    {
        exception = true;
    }
}

NemesisInfo::NemesisInfo(bool& exception)
{
    try
    {
        setup();
    }
    catch (nemesis::exception&)
    {
        exception = true;
    }
}

void NemesisInfo::ReadNemesisInfoFile()
{
    if (!sf::exists("nemesis.ini")) return;

    try
    {
        VecWstr storeline;
        SetWstr hasAuto;

        const UMap<std::wstring, std::function<void(const std::wstring&)>> config_map = {
            {L"MaxAnimation", [&](std::wstring path) { maxAnim = stoi(path); }},
            {L"First", [&](std::wstring path) { first = path != L"false"; }},
            {L"Height", [&](std::wstring path) { height = stoi(path); }},
            {L"Width", [&](std::wstring path) { width = stoi(path); }},
            {L"ModNameWidth", [&](std::wstring path) { modNameWidth = stoi(path); }},
            {L"AuthorWidth", [&](std::wstring path) { authorWidth = stoi(path); }},
            {L"PriorityWidth", [&](std::wstring path) { priorityWidth = stoi(path); }},
            {L"TimeOut", [&](std::wstring path) { timeout_timer = stoi(path); }},
            {L"SkyrimDataDirectory",
             [&](std::wstring path)
             {
                 if (!sf::exists(path)) return;

                 dataPath       = path;
                 forceDirectory = true;
                 dataPath.make_preferred();

                 VecStr filelist;
                 sf::path fspath(dataPath);

                 while (!nemesis::iequals(fspath.stem().string(), "data"))
                 {
                     fspath = fspath.parent_path();
                 }

                 read_directory(fspath.parent_path().string(), filelist);

                 for (auto& file : filelist)
                 {
                     if (nemesis::iequals(file, "SkyrimSE.exe") || nemesis::iequals(file, "binkw64.dll"))
                     {
                         SSE = true;
                         return;
                     }

                     if (nemesis::iequals(file, "binkw32.dll")) return;
                 }
             }},
        };

        if (!GetFileLines(L"nemesis.ini", storeline)) return;

        for (auto& line : storeline)
        {
            nemesis::wsmatch match;

            if (!nemesis::wregex_match(line, match, L"^([^\\s]+)[\\s]*=[\\s]*([^\\s]+)$")) continue;

            std::wstring key = match[1];
            std::wstring val = match[2];

            if (nemesis::iequals(val, L"auto")) continue;

            auto itr = config_map.find(key);

            if (itr == config_map.end()) continue;

            itr->second(val);
        }
    }
    catch (const std::exception&)
    {
        dataPath.clear();
    }
}

void NemesisInfo::SetDataPath()
{
    if (!dataPath.empty()) return;

    sf::path curpath    = sf::current_path();
    wstring curpath_str = curpath.wstring();

    VecWstr filelist;
    wstring skyrimDataDirect;

    {
        sf::path path  = curpath;
        size_t counter = count(curpath_str.begin(), curpath_str.end(), L'\\');
        size_t i       = 0;

        while (i < counter)
        {
            if (nemesis::iequals(path.stem().wstring(), L"data"))
            {
                skyrimDataDirect = path.wstring();
                break;
            }

            path = path.parent_path();
            ++i;
        }

        read_directory(path.parent_path().wstring(), filelist);
    }

    for (auto& file : filelist)
    {
        if (nemesis::iequals(file, L"SkyrimSE.exe"))
        {
            SSE = true;
            break;
        }
        else if (nemesis::iequals(file, L"binkw64.dll"))
        {
            SSE = true;
            break;
        }
        else if (nemesis::iequals(file, L"binkw32.dll"))
        {
            break;
        }
    }

    // get skyrim data directory from registry key
    DWORD dwType = REG_SZ;
    HKEY hKey    = 0;
    wchar_t value[1024];
    DWORD value_length = 1024;

    if (SSE)
    {
        RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Bethesda Softworks\\Skyrim Special Edition", &hKey);
    }
    else
    {
        RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Bethesda Softworks\\Skyrim", &hKey);
    }

    LONG result = RegQueryValueEx(hKey, L"installed path", NULL, &dwType, (LPBYTE) &value, &value_length);

    dataPath = value;
    dataPath /= L"Data\\";

    if (result == ERROR_SUCCESS && sf::exists(dataPath)) return;

    if (SSE)
    {
        RegOpenKey(
            HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Bethesda Softworks\\Skyrim Special Edition", &hKey);
    }
    else
    {
        RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Bethesda Softworks\\Skyrim", &hKey);
    }

    result   = RegQueryValueEx(hKey, L"installed path", NULL, &dwType, (LPBYTE) &value, &value_length);
    dataPath = value;
    dataPath /= L"Data\\";

    // data path directly from address
    if (result == ERROR_SUCCESS && sf::exists(dataPath)) return;

    dataPath = skyrimDataDirect;

    if (dataPath.wstring().back() != L'\\')
    {
        dataPath /= L"\\";
        return;
    }

    interMsg(dataPath.wstring());
    ErrorMessage(6005);
}

void NemesisInfo::PathValidation()
{
    wstring curpath_str = sf::current_path().wstring();

    if (nemesis::isearch(curpath_str, L"\\data\\") != NOT_FOUND) return;

    interMsg(L"Detected Path: " + curpath_str);
    DebugLogging(L"Detected Path: " + curpath_str);
    exception = true;
    ErrorMessage(1008);
}

void NemesisInfo::iniFileUpdate()
{
    QFile file("nemesis.ini");

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) return;

    QTextStream stream(&file);
    stream << QString::fromStdWString(L"SkyrimDataDirectory=" + dataPath.wstring() + L"\r\n");
    stream << QString::fromStdWString(L"MaxAnimation=" + to_wstring(maxAnim) + L"\r\n");
    stream << QString::fromStdWString(L"first=" + wstring(first ? L"true" : L"false") + L"\r\n");
    stream << QString::fromStdWString(L"width=" + to_wstring(width) + L"\r\n");
    stream << QString::fromStdWString(L"height=" + to_wstring(height) + L"\r\n");
    stream << QString::fromStdWString(L"modNameWidth=" + to_wstring(modNameWidth) + L"\r\n");
    stream << QString::fromStdWString(L"authorWidth=" + to_wstring(authorWidth) + L"\r\n");
    stream << QString::fromStdWString(L"priorityWidth=" + to_wstring(priorityWidth) + L"\r\n");
    stream.flush();
    file.close();
}

std::filesystem::path NemesisInfo::GetDataPath() const
{
    return dataPath;
}

std::filesystem::path NemesisInfo::GetStagePath() const
{
    return stageDirectory;
}

size_t NemesisInfo::GetMaxAnim() const
{
    return maxAnim;
}

size_t NemesisInfo::GetWidth() const
{
    return width;
}

size_t NemesisInfo::GetHeight() const
{
    return height;
}

size_t NemesisInfo::GetModNameWidth() const
{
    return modNameWidth;
}

size_t NemesisInfo::GetAuthorWidth() const
{
    return authorWidth;
}

size_t NemesisInfo::GetPriorityWidth() const
{
    return priorityWidth;
}

size_t NemesisInfo::GetTimeout() const
{
    return timeout_timer;
}

bool NemesisInfo::IsFirst() const
{
    return first;
}

bool NemesisInfo::HasException() const
{
    return exception;
}

void NemesisInfo::SetFirst(bool _first)
{
    first = _first;
}

void NemesisInfo::SetWidth(size_t _width)
{
    width = _width;
}

void NemesisInfo::SetHeight(size_t _height)
{
    height = _height;
}

void NemesisInfo::SetModNameWidth(size_t _width)
{
    modNameWidth = _width;
}

void NemesisInfo::SetAuthorWidth(size_t _width)
{
    authorWidth = _width;
}

void NemesisInfo::SetPriorityWidth(size_t _width)
{
    priorityWidth = _width;
}

NemesisInfo* NemesisInfo::GetInstance()
{
    if (nemesisinfo) return nemesisinfo;

    while (lock.test_and_set());

    if (nemesisinfo) return nemesisinfo;

    nemesisinfo = new NemesisInfo();
    return nemesisinfo;
}
