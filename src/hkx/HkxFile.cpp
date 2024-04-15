#include "hkx/HkxFile.h"

#include "core/CoreObject.h"

#include "utilities/writetextfile.h"

#include "nemesisinfo.h"

void nemesis::HkxFile::CompileTemplate(const nemesis::AnimationRequest* request,
                                       DeqNstr& lines,
                                       nemesis::CompileState& state) const
{
    auto list_itr = TemplateMap.find(request->GetTemplateName());

    if (list_itr == TemplateMap.end()) return;

    auto obj_itr = list_itr->second.find(request->GetLevel());

    if (obj_itr == list_itr->second.end()) return;

    state.SetBaseRequest(request);
    obj_itr->second->CompileTo(lines, state);

    auto requests = request->GetRequests();

    for (auto& request : requests)
    {
        CompileTemplate(request, lines, state);
    }
}

DeqNstr nemesis::HkxFile::CompileAllTemplates(nemesis::CompileState& state) const
{
    DeqNstr template_lines;

    for (auto& templt_list : TemplateMap)
    {
        state.ClearAllConditionCache();
        auto& requests = state.GetRequests(templt_list.first);

        if (!requests.empty())
        {
            auto itr = templt_list.second.find(0);

            if (itr != templt_list.second.end())
            {
                itr->second->CompileTo(template_lines, state);
            }
        }

        for (auto& request : requests)
        {
            CompileTemplate(request, template_lines, state);
        }
    }

    state.ClearAllConditionCache();
    return template_lines;
}

DeqNstr nemesis::HkxFile::CompileAllSubTemplates(nemesis::CompileState& state) const
{
    DeqNstr lines;
    auto subrequests = state.GetSubTemplateRequestList();

    if (subrequests.empty()) return lines;

    Vec<UPtr<nemesis::CompileState>> state_list;
    USetStr checker;

    for (auto& request : subrequests)
    {
        auto new_state = state.Clone();
        new_state->SetCurrentSubTemplateRequest(*request);
        auto template_name   = request->GetArgument(1);
        auto sub_templt_list = state.GetManager().GetTemplateRepository().GetSubTemplateList();
        const nemesis::SubTemplateObject* st_obj = nullptr;

        for (auto& each : sub_templt_list)
        {
            if (!nemesis::iequals(each->GetName(), template_name)) continue;

            st_obj = each;
            break;
        }

        if (!st_obj)
        {
            throw std::runtime_error("SubTemplate cannot be found (\"" + template_name + "\")");
        }

        checker.insert(request->GetArgument(0));
        st_obj->CompileTo(lines, *new_state);
        lines.emplace_back("");

        if (new_state->GetSubTemplateRequestList().empty()) continue;

        state_list.emplace_back(std::move(new_state));
    }

    for (size_t i = 0; i < state_list.size(); ++i)
    {
        auto cur_state = state_list[i].get();

        for (auto& request : cur_state->GetSubTemplateRequestList())
        {
            auto& id = request->GetArgument(0);

            if (checker.find(id) != checker.end()) continue;

            auto new_state = state.Clone();
            new_state->SetCurrentSubTemplateRequest(*request);

            auto template_name   = request->GetArgument(1);
            auto sub_templt_list = state.GetManager().GetTemplateRepository().GetSubTemplateList();
            const nemesis::SubTemplateObject* st_obj = nullptr;

            for (auto& each : sub_templt_list)
            {
                if (!nemesis::iequals(each->GetName(), template_name)) continue;

                st_obj = each;
                break;
            }

            if (!st_obj)
            {
                throw std::runtime_error("SubTemplate cannot be found (\"" + template_name + "\")");
            }

            checker.insert(id);
            st_obj->CompileTo(lines, *new_state);
            lines.emplace_back("");

            if (new_state->GetSubTemplateRequestList().empty()) continue;

            state_list.emplace_back(std::move(new_state));
        }
    }

    return lines;
}

void nemesis::HkxFile::CompileFile(nemesis::CompileState& state) const
{
    CompileFileAs(NemesisInfo::GetInstance()->PatchOutputPath(TargetPath), state);
}

void nemesis::HkxFile::CompileFileAs(const std::filesystem::path& filepath,
                                     nemesis::CompileState& state) const
{
    DeqNstr lines = Compile(state);
    FileWriter writer(filepath);

    for (auto& line : lines)
    {
        writer.LockFreeWriteLine(line);
    }
}

void nemesis::HkxFile::AddTemplate(const SPtr<nemesis::TemplateObject>& templt_obj)
{
    auto& class_name  = templt_obj->GetClassName();
    std::string level = templt_obj->GetFilePath().stem().string().substr(class_name.length() + 1);
    TemplateMap[templt_obj->GetClassName()][std::stoi(level)] = templt_obj;
}

nemesis::HkxNode* nemesis::HkxFile::AddModNode(const std::string& modcode, UPtr<nemesis::HkxNode>&& node)
{
    nemesis::SemanticManager manager;
    auto* node_ptr = node.get();
    auto mod_obj
        = std::make_unique<nemesis::ModObject>(modcode, 0, node->GetFilePath(), manager, std::move(node));
    NewNodes->AddObject(std::move(mod_obj));

    auto& mod_list = manager.GetModInUsedList();
    ModInUsedList.insert(mod_list.begin(), mod_list.end());
    return node_ptr;
}

nemesis::HkxNode* nemesis::HkxFile::GetNodeById(const std::string& node_id)
{
    auto itr = NodeMap.find(node_id);

    if (itr == NodeMap.end()) return nullptr;

    return itr->second;
}

const nemesis::HkxNode* nemesis::HkxFile::GetNodeById(const std::string& node_id) const
{
    auto itr = NodeMap.find(node_id);

    if (itr == NodeMap.end()) return nullptr;

    return itr->second;
}

const std::filesystem::path& nemesis::HkxFile::GetFilePath() const noexcept
{
    return FilePath;
}

const std::filesystem::path& nemesis::HkxFile::GetTargetPath() const noexcept
{
    return TargetPath;
}

const std::filesystem::path& nemesis::HkxFile::GetCachedFilePath() const noexcept
{
    return CachedFilePath;
}

const std::filesystem::path& nemesis::HkxFile::GetRelativePath() const noexcept
{
    return RelativePath;
}

bool nemesis::HkxFile::IsSameAsCached(nemesis::CompileState& state) const
{
    if (!std::filesystem::exists(CachedFilePath)) return false;

    auto mods = state.GetSelectedMods();

    for (auto& mod : mods)
    {
        if (ModInUsedList.find(mod) != ModInUsedList.end()) return false;
    }

    for (auto& templt_list : TemplateMap)
    {
        auto& requests = state.GetRequests(templt_list.first);

        if (!requests.empty()) return false;
    }

    return true;
}

bool nemesis::HkxFile::TryGetValueInHkcString(const std::string& line, std::string& value)
{
    size_t pos = line.find("<hkcstring>");

    if (pos == NOT_FOUND) return false;

    size_t cpos = line.find("</hkcstring>", pos);

    if (cpos == NOT_FOUND) return false;

    value = line.substr(pos + 11, cpos - pos - 11);
    return true;
}
