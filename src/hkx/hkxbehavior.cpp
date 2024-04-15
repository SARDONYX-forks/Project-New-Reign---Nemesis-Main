#include "hkx/HkxBehavior.h"

#include "core/IfObject.h"
#include "core/CompileState.h"
#include "core/AnimationRequestCollection.h"

#include "nemesisinfo.h"

UPtr<nemesis::CollectionObject>
nemesis::HkxBehavior::ParseDataNodes(nemesis::LineStream& stream,
                                     nemesis::SemanticManager& manager,
                                     Map<std::string, nemesis::HkxNode*>& node_map)
{
    auto collection = std::make_unique<nemesis::CollectionObject>();

    while (!stream.IsEoF())
    {
        if (!nemesis::HkxNode::IsDataClass(stream)) break;

        nemesis::HkxNode* node_ptr;
        auto node         = nemesis::HkxNode::ParseHkxNode(stream, manager, node_ptr);
        node_map[node_ptr->GetNodeId()] = node_ptr;
        collection->AddObject(std::move(node));
    }

    return collection;
}

UPtr<nemesis::CollectionObject>
nemesis::HkxBehavior::ParseRegularNodes(nemesis::LineStream& stream,
                                        nemesis::SemanticManager& manager,
                                        Map<std::string, nemesis::HkxNode*>& node_map)
{
    auto collection = std::make_unique<nemesis::CollectionObject>();

    while (!stream.IsEoF())
    {
        if ((*stream).find("</hksection>") != NOT_FOUND) return collection;

        nemesis::HkxNode* node_ptr;
        auto node = nemesis::HkxNode::ParseHkxNode(stream, manager, node_ptr);
        node_map[node_ptr->GetNodeId()] = node_ptr;
        collection->AddObject(std::move(node));
    }

    return collection;
}

void nemesis::HkxBehavior::CompileTo(DeqNstr& lines, nemesis::CompileState& state) const
{
    lines.emplace_back(R"(<?xml version="1.0" encoding="ascii"?>)", 1);
    lines.emplace_back(R"(<hkpackfile classversion="8" contentsversion="hk_2010.2.0 - r1" toplevelobject=")"
                           + RootNode->GetNodeId() + R"(">)",
                       2);
    lines.emplace_back("", 3);
    lines.emplace_back(R"(	<hksection name="__data__">)", 4);
    lines.emplace_back("", 5);

    UPtr<void*> uhandler_add = std::make_unique<void*>(nullptr);
    void** handler_add       = uhandler_add.get();

    auto property_add = std::make_shared<std::function<void(nemesis::Line&)>>(
        [handler_add, &state](nemesis::Line& line)
        {
            if (line.find("</hkparam>") != NOT_FOUND)
            {
                state.RemoveAddLineHandler(*handler_add);
                return;
            }

            std::string str_value;

            if (!TryGetValueInHkcString(line, str_value)) return;

            state.AddPropertyName(str_value);
        });
    
    auto variable_add = std::make_shared<std::function<void(nemesis::Line&)>>(
        [handler_add, &state, property_add](nemesis::Line& line)
        {
            if (line.find(R"(<hkparam name="characterPropertyNames" numelements=)") != NOT_FOUND)
            {
                auto address = *handler_add;
                *handler_add = state.InsertAddLineHandler(*property_add);
                state.RemoveAddLineHandler(address);
                return;
            }

            std::string str_value;

            if (!TryGetValueInHkcString(line, str_value)) return;

            state.AddVariableName(str_value);
        });
    
    auto attribute_add = std::make_shared<std::function<void(nemesis::Line&)>>(
        [handler_add, &state, variable_add](nemesis::Line& line)
        {
            if (line.find(R"(<hkparam name="variableNames" numelements=)") != NOT_FOUND)
            {
                auto address = *handler_add;
                *handler_add = state.InsertAddLineHandler(*variable_add);
                state.RemoveAddLineHandler(address);
                return;
            }

            std::string str_value;

            if (!TryGetValueInHkcString(line, str_value)) return;

            state.AddAttributeName(str_value);
        });

    auto event_add = std::make_shared<std::function<void(nemesis::Line&)>>(
        [handler_add, &state, attribute_add](nemesis::Line& line)
        {
            if (line.find(R"(<hkparam name="attributeNames" numelements=)") != NOT_FOUND)
            {
                auto address = *handler_add;
                *handler_add = state.InsertAddLineHandler(*attribute_add);
                state.RemoveAddLineHandler(address);
                return;
            }

            std::string str_value;

            if (!TryGetValueInHkcString(line, str_value)) return;

            state.AddEventName(str_value);
        });

    (*handler_add) = state.InsertAddLineHandler(
        [handler_add, &state, event_add](nemesis::Line& line)
        {
            if (line.find(R"(<hkparam name="eventNames" numelements=)") == NOT_FOUND) return;

            auto address = *handler_add;
            *handler_add = state.InsertAddLineHandler(*event_add);
            state.RemoveAddLineHandler(address);
        });

    DeqNstr behavior_lines;
    DataNodes->CompileTo(lines, state);
    lines.emplace_back("", lines.size() + 1);
    NewNodes->CompileTo(behavior_lines, state);
    lines.emplace_back("", behavior_lines.size() + 1);
    RegularNodes->CompileTo(behavior_lines, state);
    lines.emplace_back("", behavior_lines.size() + 1);

    DeqNstr template_lines    = CompileAllTemplates(state);
    DeqNstr subtemplate_lines = CompileAllSubTemplates(state);

    lines.insert(lines.end(),
                 std::make_move_iterator(subtemplate_lines.begin()),
                 std::make_move_iterator(subtemplate_lines.end()));
    lines.insert(lines.end(),
                 std::make_move_iterator(template_lines.begin()),
                 std::make_move_iterator(template_lines.end()));
    lines.insert(lines.end(),
                 std::make_move_iterator(behavior_lines.begin()),
                 std::make_move_iterator(behavior_lines.end()));

    lines.emplace_back("	</hksection>", lines.size() + 1);
    lines.emplace_back("", lines.size() + 1);
    lines.emplace_back("</hkpackfile>", lines.size() + 1);
}

void nemesis::HkxBehavior::SerializeTo(DeqNstr& lines) const
{
    lines.emplace_back(R"(<?xml version="1.0" encoding="ascii"?>)", 1);
    lines.emplace_back(R"(<hkpackfile classversion="8" contentsversion="hk_2010.2.0 - r1" toplevelobject=")"
                           + RootNode->GetNodeId() + R"(">)",
                       2);
    lines.emplace_back("", 3);
    lines.emplace_back(R"(	<hksection name="__data__">)", 4);
    lines.emplace_back("", 5);

    DataNodes->SerializeTo(lines);
    lines.emplace_back("", lines.size() + 1);
    NewNodes->SerializeTo(lines);
    lines.emplace_back("", lines.size() + 1);
    RegularNodes->SerializeTo(lines);
    lines.emplace_back("", lines.size() + 1);

    lines.emplace_back("	</hksection>", lines.size() + 1);
    lines.emplace_back("", lines.size() + 1);
    lines.emplace_back("</hkpackfile>", lines.size() + 1);
}

UPtr<nemesis::NObject> nemesis::HkxBehavior::CloneNObject() const
{
    return Clone();
}

UPtr<nemesis::HkxBehavior> nemesis::HkxBehavior::Clone() const
{
    throw std::runtime_error("nemesis::HkxBehavior cannot be cloned");
}

UPtr<nemesis::HkxBehavior> nemesis::HkxBehavior::ParseFromFile(const std::filesystem::path& filepath)
{
    UPtr<nemesis::HkxBehavior> hkxfile_uptr(new nemesis::HkxBehavior());
    nemesis::HkxBehavior* hkxfile_ptr = hkxfile_uptr.get();
    hkxfile_ptr->FilePath   = filepath;
    hkxfile_ptr->TargetPath = filepath.parent_path()
                              / (nemesis::istarts_with(filepath.filename().wstring(), L"nemesis_")
                                     ? filepath.filename().wstring().substr(8)
                                     : filepath.filename());
    hkxfile_ptr->RelativePath = hkxfile_ptr->TargetPath.wstring().substr(
        NemesisInfo::GetInstance()->GetDataPath().wstring().length() + 1);

    auto cache_path = filepath.parent_path() / (filepath.filename().wstring() + L".cache");

    if (std::filesystem::exists(cache_path))
    {
        hkxfile_ptr->CachedFilePath = cache_path;
    }

    VecNstr lines;
    GetFileLines(filepath, lines, false);
    auto start_itr = lines.begin();

    while (!nemesis::regex_match(*start_itr, nemesis::HkxNode::NodeIdRgx))
    {
        ++start_itr;
    }

    nemesis::SemanticManager manager;
    nemesis::LineStream stream(start_itr, lines.end());

    hkxfile_ptr->DataNodes     = ParseDataNodes(stream, manager, hkxfile_ptr->NodeMap);
    hkxfile_ptr->NewNodes      = std::make_unique<nemesis::CollectionObject>();
    hkxfile_ptr->RegularNodes  = ParseRegularNodes(stream, manager, hkxfile_ptr->NodeMap);
    hkxfile_ptr->ModInUsedList = manager.GetModInUsedList();

    for (auto& node_pair : hkxfile_ptr->NodeMap)
    {
        if (node_pair.second->GetClassName() != "hkRootLevelContainer") continue;

        hkxfile_ptr->RootNode = node_pair.second;
        break;
    }

    return hkxfile_uptr;
}

UPtr<nemesis::HkxBehavior> nemesis::HkxBehavior::ParseFromFile(const std::filesystem::path& filepath,
                                                                nemesis::ThreadPool& thread_pool)
{
    UPtr<nemesis::HkxBehavior> hkxfile_uptr(new nemesis::HkxBehavior());
    nemesis::HkxBehavior* hkxfile_ptr = hkxfile_uptr.get();
    hkxfile_ptr->FilePath             = filepath;
    hkxfile_ptr->TargetPath           = filepath.parent_path()
                              / (nemesis::istarts_with(filepath.filename().wstring(), L"nemesis_")
                                     ? filepath.filename().wstring().substr(8)
                                     : filepath.filename());
    hkxfile_ptr->RelativePath = hkxfile_ptr->TargetPath.wstring().substr(
        NemesisInfo::GetInstance()->GetDataPath().wstring().length() + 1);

    auto cache_path = filepath.parent_path() / (filepath.filename().wstring() + L".cache");

    if (std::filesystem::exists(cache_path))
    {
        hkxfile_ptr->CachedFilePath = cache_path;
    }

    thread_pool.enqueue(
        [hkxfile_ptr]()
        {
            VecNstr lines;
            GetFileLines(hkxfile_ptr->FilePath, lines, false);
            auto start_itr = lines.begin();

            while (!nemesis::regex_match(*start_itr, nemesis::HkxNode::NodeIdRgx))
            {
                ++start_itr;
            }

            nemesis::SemanticManager manager;
            nemesis::LineStream stream(start_itr, lines.end());

            hkxfile_ptr->DataNodes     = ParseDataNodes(stream, manager, hkxfile_ptr->NodeMap);
            hkxfile_ptr->NewNodes      = std::make_unique<nemesis::CollectionObject>();
            hkxfile_ptr->RegularNodes  = ParseRegularNodes(stream, manager, hkxfile_ptr->NodeMap);
            hkxfile_ptr->ModInUsedList = manager.GetModInUsedList();

            for (auto& node_pair : hkxfile_ptr->NodeMap)
            {
                if (node_pair.second->GetClassName() != "hkRootLevelContainer") continue;

                hkxfile_ptr->RootNode = node_pair.second;
                break;
            }
        });

    return hkxfile_uptr;
}
