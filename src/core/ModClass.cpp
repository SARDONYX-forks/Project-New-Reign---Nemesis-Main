#include "core/ModClass.h"

#include "animdata/AnimationDataProject.h"

#include "utilities/algorithm.h"

void nemesis::ModClass::ParseModFromMeshesDirectory(const std::filesystem::path& meshes_dir_path)
{
    for (auto& entry : std::filesystem::directory_iterator(meshes_dir_path))
    {
        if (!entry.is_directory())
        {
            auto node = nemesis::HkxNode::ParseHkxNodeFromFile(entry.path());
            NodeList.emplace_back(
                std::make_unique<nemesis::ModPatch<nemesis::HkxNode>>(*this, std::move(node)));
            continue;
        }

        auto path = entry.path();

        if (nemesis::iequals(path.stem().wstring(), L"animationdatasinglefile"))
        {
            ParseModAnimDataFromDirectory(path);
            continue;
        }
        
        if (nemesis::iequals(path.stem().wstring(), L"animationsetdatasinglefile"))
        {
            ParseModAnimSetDataFromDirectory(path);
            continue;
        }

        ParseModHkxFromDirectory(path);
    }
}

void nemesis::ModClass::ParseModHkxFromDirectory(const std::filesystem::path& dir_path)
{
    for (auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        if (entry.is_directory())
        {
            ParseModHkxFromDirectory(entry.path());
            continue;
        }

        auto node = nemesis::HkxNode::ParseHkxNodeFromFile(entry.path());
        NodeList.emplace_back(std::make_unique<nemesis::ModPatch<nemesis::HkxNode>>(*this, std::move(node)));
    }
}

void nemesis::ModClass::ParseModAnimDataFromDirectory(const std::filesystem::path& dir_path) 
{
    for (auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        if (entry.is_directory())
        {
            ParseModAnimDataFromDirectory(entry.path());
            continue;
        }

        std::filesystem::path path = entry.path();
        std::wstring filename      = path.stem().wstring();

        if (nemesis::iequals(filename, L"$header$"))
        {
            auto header = nemesis::AnimationDataProject::DeserializeHeaderFromFile(path);
            AnimDataHeaderList.emplace_back(
                std::make_unique<nemesis::ModPatch<nemesis::AnimationDataProject::Headers>>(
                    *this, std::move(header)));
            continue;
        }

        nemesis::smatch match;

        if (nemesis::regex_match(path.stem().string(), match, ModRgx))
        {
            if (match[1].empty())
            {
                auto motion_data = nemesis::AnimationDataMotionData::DeserializeFromFile(path);
                MotionDataList.emplace_back(
                    std::make_unique<nemesis::ModPatch<nemesis::AnimationDataMotionData>>(
                        *this, std::move(motion_data)));
                continue;
            }

            auto clip_data = nemesis::AnimationDataClipData::DeserializeFromFile(path);
            ClipDataList.emplace_back(std::make_unique<nemesis::ModPatch<nemesis::AnimationDataClipData>>(
                *this, std::move(clip_data)));
            continue;
        }

        if (isOnlyNumber(filename))
        {
            auto motion_data = nemesis::AnimationDataMotionData::DeserializeFromFile(path);
            MotionDataList.emplace_back(std::make_unique<nemesis::ModPatch<nemesis::AnimationDataMotionData>>(
                *this, std::move(motion_data)));
            continue;
        }

        auto clip_data = nemesis::AnimationDataClipData::DeserializeFromFile(path);
        ClipDataList.emplace_back(std::make_unique<nemesis::ModPatch<nemesis::AnimationDataClipData>>(*this, std::move(clip_data)));
    }
}

void nemesis::ModClass::ParseModAnimSetDataFromDirectory(const std::filesystem::path& dir_path)
{
    for (auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        if (entry.is_directory())
        {
            ParseModAnimSetDataFromDirectory(entry.path());
            continue;
        }

        auto state = nemesis::AnimationSetDataState::DeserializeFromFile(entry.path());
        StateList.emplace_back(
            std::make_unique<nemesis::ModPatch<nemesis::AnimationSetDataState>>(*this, std::move(state)));
    }
}

nemesis::ModClass::ModClass(const std::filesystem::path& ini_file)
{
    auto parent_path = ini_file.parent_path();
    Info.ReadFile(ini_file);
    Info.SetModCode(parent_path.stem().wstring());
    ModRgx = nemesis::regex("^(.+?~|)" + parent_path.stem().string() + "\\$[0-9]+$");

    for (auto& entry : std::filesystem::directory_iterator(parent_path))
    {
        if (!entry.is_directory()) continue;

        ParseModFromMeshesDirectory(entry.path());
    }
}

std::string nemesis::ModClass::GetCode() const noexcept
{
    return Info.GetModCodeA();
}

Vec<const nemesis::ModPatch<nemesis::HkxNode>*> nemesis::ModClass::GetNodeList() const
{
    if (NodeListCache) return *NodeListCache;

    NodeListCache = std::make_unique<Vec<const nemesis::ModPatch<nemesis::HkxNode>*>>();

    for (auto& node : NodeList)
    {
        NodeListCache->emplace_back(node.get());
    }

    return *NodeListCache;
}

Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*> nemesis::ModClass::GetStateList() const
{
    if (StateListCache) return *StateListCache;

    StateListCache = std::make_unique<Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*>>();

    for (auto& state : StateList)
    {
        StateListCache->emplace_back(state.get());
    }

    return *StateListCache;
}

Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*>
nemesis::ModClass::GetAnimDataHeaderList() const
{
    if (AnimDataHeaderListCache) return *AnimDataHeaderListCache;

    AnimDataHeaderListCache
        = std::make_unique<Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*>>();

    for (auto& header : AnimDataHeaderList)
    {
        AnimDataHeaderListCache->emplace_back(header.get());
    }

    return *AnimDataHeaderListCache;
}

Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*> nemesis::ModClass::GetClipDataList() const
{
    if (ClipDataListCache) return *ClipDataListCache;

    ClipDataListCache = std::make_unique<Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*>>();

    for (auto& clip_data : ClipDataList)
    {
        ClipDataListCache->emplace_back(clip_data.get());
    }

    return *ClipDataListCache;
}

Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*> nemesis::ModClass::GetMotionDataList() const
{
    if (MotionDataListCache) return *MotionDataListCache;

    MotionDataListCache = std::make_unique<Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*>>();

    for (auto& motion_data : MotionDataList)
    {
        MotionDataListCache->emplace_back(motion_data.get());
    }

    return *MotionDataListCache;
}
