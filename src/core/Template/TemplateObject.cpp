#include "core/CompileState.h"
#include "core/NObjectParser.h"
#include "core/SemanticManager.h"

#include "core/Template.h"

nemesis::TemplateObject::TemplateObject(const nemesis::TemplateClass* template_class) noexcept
    : TemplateClass(template_class)
{
}

void nemesis::TemplateObject::CompileTo(DeqNstr& lines, nemesis::CompileState& state) const
{
    Data->CompileTo(lines, state);

    if (!Child) return;

    auto* base    = state.GetBaseRequest();
    auto requests = base->GetRequests();

    for (auto& request : requests)
    {
        state.SetBaseRequest(request);
        Child->CompileTo(lines, state);
    }

    state.SetBaseRequest(base);
}

void nemesis::TemplateObject::SerializeTo(DeqNstr& lines) const
{
    Data->SerializeTo(lines);
}

UPtr<nemesis::NObject> nemesis::TemplateObject::CloneNObject() const
{
    return Clone();
}

UPtr<nemesis::TemplateObject> nemesis::TemplateObject::Clone() const
{
    auto tmplt      = std::make_unique<nemesis::TemplateObject>(TemplateClass);
    tmplt->Index    = Index;
    tmplt->FilePath = FilePath;
    tmplt->Data     = !Data ? nullptr : Data->Clone();
    tmplt->Child    = !Child ? nullptr : Child->Clone();
    return tmplt;
}

const std::string& nemesis::TemplateObject::GetClassName() const noexcept
{
    return TemplateClass->GetName();
}

size_t nemesis::TemplateObject::GetIndex() const noexcept
{
    return Index;
}

const std::filesystem::path& nemesis::TemplateObject::GetFilePath() const noexcept
{
    return FilePath;
}

std::filesystem::path nemesis::TemplateObject::GetTargetPath() const noexcept
{
    auto info_path    = TemplateClass->GetInfoPath();
    size_t dir_length = info_path.parent_path().string().length();

    std::string path = FilePath.parent_path().string();
    path             = path.substr(dir_length);
    return path;
}

const nemesis::TemplateClass* nemesis::TemplateObject::GetTemplateClass() const noexcept
{
    return TemplateClass;
}

nemesis::TemplateObject* nemesis::TemplateObject::GetChild(UPtr<nemesis::TemplateObject>&& child)
{
    return Child.get();
}

void nemesis::TemplateObject::SetChild(UPtr<nemesis::TemplateObject>&& child)
{
    Child = std::move(child);
}

SPtr<nemesis::TemplateObject>
nemesis::TemplateObject::ParseFromFile(const std::filesystem::path& filepath,
                                       const nemesis::TemplateClass* templt_class)
{
    std::string name(templt_class->GetName());

    if (std::find_if(name.begin(), name.end(), [](const char& ch) { return !std::isalpha(ch); })
        != name.end())
    {
        throw std::runtime_error("Invalid template name (" + name + ")");
    }

    std::string filename = filepath.stem().string();
    nemesis::regex name_rgx("(" + name + ")_([0-9]+)");
    nemesis::smatch match;

    if (!nemesis::regex_match(filename, match, name_rgx))
    {
        throw std::runtime_error("Invalid filename "
                                 "(Template: "
                                 + name + ", File: " + filepath.string() + ")");
    }

    auto templt_ptr = new nemesis::TemplateObject(templt_class);
    SPtr<nemesis::TemplateObject> templt(templt_ptr);
    templt->Index    = std::stoul(match[2]);
    templt->Data     = std::make_unique<nemesis::CollectionObject>();
    templt->FilePath = filepath;

    auto& data_ref = *templt_ptr->Data;
    nemesis::SemanticManager manager;
    manager.SetCurrentTemplate(templt_ptr);

    for (size_t i = 1; i <= templt->Index; i++)
    {
        manager.TryAddRequestToQueue(name + "_" + std::to_string(i));
    }

    VecNstr lines;
    GetFileLines(filepath, lines, false);

    for (nemesis::LineStream stream(lines.begin(), lines.end()); !stream.IsEoF(); ++stream)
    {
        data_ref.AddObject(nemesis::NObjectParser::ParseLine(stream, manager));
    }

    return templt;
}

SPtr<nemesis::TemplateObject>
nemesis::TemplateObject::ParseFromFile(const std::filesystem::path& filepath,
                                       const nemesis::TemplateClass* templt_class,
                                       nemesis::ThreadPool& thread_pool)
{
    std::string name(templt_class->GetName());

    if (std::find_if(name.begin(), name.end(), [](const char& ch) { return !std::isalpha(ch); })
        != name.end())
    {
        throw std::runtime_error("Invalid template name (" + name + ")");
    }

    std::string filename = filepath.stem().string();
    nemesis::regex name_rgx("(" + name + ")_([0-9]+)");
    nemesis::smatch match;

    if (!nemesis::regex_match(filename, match, name_rgx))
    {
        throw std::runtime_error("Invalid filename "
                                 "(Template: "
                                 + name + ", File: " + filepath.string() + ")");
    }

    auto templt_ptr = new nemesis::TemplateObject(templt_class);
    SPtr<nemesis::TemplateObject> templt(templt_ptr);
    templt->Index    = std::stoul(match[2]);
    templt->Data     = std::make_unique<nemesis::CollectionObject>();
    templt->FilePath = filepath;

    thread_pool.enqueue(
        [templt_ptr]()
        {
            auto& data_ref = *templt_ptr->Data;
            nemesis::SemanticManager manager;
            manager.SetCurrentTemplate(templt_ptr);

            VecNstr lines;
            GetFileLines(templt_ptr->FilePath, lines, false);

            for (nemesis::LineStream stream(lines.begin(), lines.end()); !stream.IsEoF(); ++stream)
            {
                data_ref.AddObject(nemesis::NObjectParser::ParseLine(stream, manager));
            }
        });

    return templt;
}
