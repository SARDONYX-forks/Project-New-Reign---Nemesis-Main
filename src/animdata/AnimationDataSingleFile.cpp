#include "animdata/AnimationDataSingleFile.h"

#include "core/SemanticManager.h"
#include "core/CollectionObject.h"

#include "nemesisinfo.h"

#include "utilities/writetextfile.h"

VecNstr nemesis::AnimationDataSingleFile::ParseHeaders(nemesis::LineStream& stream,
                                                       nemesis::SemanticManager& manager)
{
    VecNstr headers;

    if (stream.IsEoF()) return headers;

    auto& token = stream.GetToken();
    auto& value = token.Value;

    if (token.Type != nemesis::LineStream::NONE || !isOnlyNumber(value))
    {
        throw std::runtime_error("nemesis::AnimationDataSingleFile::ParseHeaders format error (Line: "
                                 + std::to_string(value.GetLineNumber())
                                 + ". File: " + value.GetFilePath().string() + ")");
    }

    for (++stream; !stream.IsEoF() && !isOnlyNumber(stream.GetToken().Value); ++stream)
    {
        auto& token = stream.GetToken();

        if (token.Type != nemesis::LineStream::NONE)
        {
            auto& token_value = stream.GetToken().Value;
            throw std::runtime_error("Syntax Error: Unsupport syntax (Line: "
                                     + std::to_string(token_value.GetLineNumber())
                                     + ". File: " + token_value.GetFilePath().string() + ")");
        }

        headers.emplace_back(token.Value);
    }

    return headers;
}

void nemesis::AnimationDataSingleFile::CompileTo(DeqNstr& lines, nemesis::CompileState& state) const
{
    DeqNstr header_lines;
    DeqNstr project_contents;

    for (auto& project : ProjectList)
    {
        DeqNstr temp_lines;
        project->CompileTo(temp_lines, state);

        if (temp_lines.empty()) continue;

        header_lines.emplace_back(project->GetName());

        for (auto& line : temp_lines)
        {
            project_contents.emplace_back(std::move(line));
        }
    }

    lines.emplace_back(std::to_string(header_lines.size()));

    for (auto& line : header_lines)
    {
        lines.emplace_back(std::move(line));
    }

    for (auto& line : project_contents)
    {
        lines.emplace_back(std::move(line));
    }
}

void nemesis::AnimationDataSingleFile::SerializeTo(DeqNstr& lines) const
{
    throw std::runtime_error("nemesis::AnimationDataSingleFile::SerializeTo is not supported");
}

UPtr<nemesis::NObject> nemesis::AnimationDataSingleFile::CloneNObject() const
{
    return Clone();
}

UPtr<nemesis::AnimationDataSingleFile> nemesis::AnimationDataSingleFile::Clone() const
{
    auto singlefile = std::make_unique<nemesis::AnimationDataSingleFile>();
    
    for (auto& project : ProjectList)
    {
        singlefile->ProjectList.emplace_back(project->Clone());
    }

    return singlefile;
}

void nemesis::AnimationDataSingleFile::CompileFile(nemesis::CompileState& state) const
{
    CompileFileAs(TargetPath, state);
}

void nemesis::AnimationDataSingleFile::CompileFileAs(const std::filesystem::path& filepath,
                                                     nemesis::CompileState& state) const
{
    DeqNstr lines = Compile(state);
    FileWriter writer(filepath);

    for (auto& line : lines)
    {
        writer.LockFreeWriteLine(line);
    }
}

std::filesystem::path nemesis::AnimationDataSingleFile::GetFilePath() const
{
    return FilePath;
}

std::filesystem::path nemesis::AnimationDataSingleFile::GetTargetPath() const
{
    return TargetPath;
}

std::filesystem::path nemesis::AnimationDataSingleFile::GetRelativePath() const
{
    return RelativePath;
}

nemesis::AnimationDataProject* nemesis::AnimationDataSingleFile::GetProject(const std::string& project_name,
                                                                            size_t order_position)
{
    size_t cur_order = 0;

    for (auto& project : ProjectList)
    {
        if (project->GetName() != project_name) continue;

        if (++cur_order == order_position) return project.get();
    }

    return nullptr;
}

UPtr<nemesis::AnimationDataProject>&
nemesis::AnimationDataSingleFile::AddProject(UPtr<nemesis::AnimationDataProject>&& project)
{
    std::scoped_lock<std::mutex> lock(UpdaterMutex);
    return ProjectList.emplace_back(std::move(project));
}

SPtr<nemesis::TemplateObject>&
nemesis::AnimationDataSingleFile::AddProjectTemplate(const SPtr<nemesis::TemplateObject>& templt_obj)
{
    return ProjectTemplateList.emplace_back(templt_obj);
}

void nemesis::AnimationDataSingleFile::SerializeToDirectory(const std::filesystem::path& directory_path) const
{
    std::filesystem::create_directories(directory_path);

    for (size_t i = 0; i < ProjectList.size(); i++)
    {
        auto& project = ProjectList[i];
        std::filesystem::path project_path = std::to_string(i + 1) + "~" + project->GetName();
        project->SerializeToDirectory(directory_path / project_path);
    }
}

UPtr<nemesis::AnimationDataSingleFile>
nemesis::AnimationDataSingleFile::DeserializeFromDirectory(const std::filesystem::path& directory_path)
{
    auto singlefile = std::make_unique<nemesis::AnimationDataSingleFile>();
    Map<size_t, std::pair<std::filesystem::path, std::string>> project_dir;

    for (auto& entry : std::filesystem::directory_iterator(directory_path))
    {
        if (!entry.is_directory()) continue;

        auto path     = entry.path();
        auto filename = path.filename().string();
        size_t pos    = filename.rfind("~");

        if (pos == NOT_FOUND) continue;

        std::string num = filename.substr(pos);
        
        if (isOnlyNumber(num)) continue;

        project_dir[std::stoul(num)] = std::make_pair(path, filename.substr(pos + 1));
    }

    for (auto& each : project_dir)
    {
        singlefile->AddProject(
            nemesis::AnimationDataProject::DeserializeFromDirectory(each.second.first, each.second.second));
    }

    return singlefile;
}

UPtr<nemesis::AnimationDataSingleFile>
nemesis::AnimationDataSingleFile::ParseFromFile(const std::filesystem::path& filepath)
{
    auto singlefile        = std::make_unique<nemesis::AnimationDataSingleFile>();
    singlefile->FilePath   = filepath;
    singlefile->TargetPath = NemesisInfo::GetInstance()->PatchOutputPath(filepath.parent_path())
                             / (nemesis::istarts_with(filepath.filename().wstring(), L"nemesis_")
                                    ? filepath.filename().wstring().substr(8)
                                    : filepath.filename());

    nemesis::SemanticManager manager;
    VecNstr lines;
    GetFileLines(filepath, lines, false);
    nemesis::LineStream stream(lines.begin(), lines.end());

    VecNstr headers = nemesis::AnimationDataSingleFile::ParseHeaders(stream, manager);
    singlefile->ProjectList = nemesis::AnimationDataProject::ParseObjects(stream, manager, headers);
    return singlefile;
}

UPtr<nemesis::AnimationDataSingleFile>
nemesis::AnimationDataSingleFile::ParseFromFile(const std::filesystem::path& filepath,
                                                nemesis::ThreadPool& threadpool)
{
    auto singlefile        = std::make_unique<nemesis::AnimationDataSingleFile>();
    singlefile->FilePath   = filepath;
    singlefile->TargetPath = filepath.parent_path()
                             / (nemesis::istarts_with(filepath.filename().wstring(), L"nemesis_")
                                    ? filepath.filename().wstring().substr(8)
                                    : filepath.filename());
    singlefile->RelativePath = singlefile->TargetPath.wstring().substr(
        NemesisInfo::GetInstance()->GetDataPath().wstring().length() + 1);
    auto singlefile_ptr = singlefile.get();

    threadpool.enqueue(
        [singlefile_ptr]()
        {
            nemesis::SemanticManager manager;
            VecNstr lines;
            GetFileLines(singlefile_ptr->GetFilePath(), lines, false);
            nemesis::LineStream stream(lines.begin(), lines.end());

            VecNstr headers = nemesis::AnimationDataSingleFile::ParseHeaders(stream, manager);
            singlefile_ptr->ProjectList
                = nemesis::AnimationDataProject::ParseObjects(stream, manager, headers);
        });
    return singlefile;
}
