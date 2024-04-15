#include "core/CompileState.h"
#include "core/AnimationRequest.h"
#include "core/CompilationManager.h"
#include "core/AnimationRequestRepository.h"

const Pair<const std::string*, size_t>&
nemesis::CompileState::GetCurrentRequestMap(const nemesis::AnimationRequest* request,
                                            const std::string& key) const
{
    auto itr = CurrentRequestMap.find(request);

    if (itr == CurrentRequestMap.end())
    {
        throw std::runtime_error("Current map index is not registered in queue. Use "
                                 "SemanticManager::HasRequestMapInQueue during parsing to "
                                 "ensure the semantic is correct");
    }

    auto k_itr = itr->second.find(key);

    if (k_itr != itr->second.end()) return k_itr->second.back();

    throw std::runtime_error("Current map index is not registered in queue. Use "
                             "SemanticManager::HasRequestMapInQueue during parsing to "
                             "ensure the semantic is correct");
}

const Pair<const std::string*, size_t>& nemesis::CompileState::GetCurrentMap(const std::string& key) const
{
    auto itr = CurrentMap.find(key);

    if (itr != CurrentMap.end()) return itr->second.back();

    throw std::runtime_error(
        "Current map index is not registered in queue. Use SemanticManager::HasMapInQueue during parsing to "
        "ensure the semantic is correct");
}

const Pair<const std::string*, size_t>&
nemesis::CompileState::GetCurrentRequestMotion(const nemesis::AnimationRequest* request) const
{
    auto itr = CurrentRequestMotionData.find(request);

    if (itr != CurrentRequestMotionData.end()) return itr->second.back();

    throw std::runtime_error("Current motion data is not registered in queue. Use "
                             "SemanticManager::HasMotionDataInQueue during parsing to "
                             "ensure the semantic is correct");
}

const Pair<const std::string*, size_t>& nemesis::CompileState::GetCurrentMotion() const
{
    if (!CurrentMotionData.empty()) return CurrentMotionData.back();

    throw std::runtime_error("Current motion data is not registered in queue. Use "
                             "SemanticManager::HasMotionDataInQueue during parsing to "
                             "ensure the semantic is correct");
}

const Pair<const std::string*, size_t>&
nemesis::CompileState::GetCurrentRequestRotation(const nemesis::AnimationRequest* request) const
{
    auto itr = CurrentRequestRotationData.find(request);

    if (itr != CurrentRequestRotationData.end()) return itr->second.back();

    throw std::runtime_error("Current rotation is not registered in queue. Use "
                             "SemanticManager::HasRotationDataInQueue during parsing to "
                             "ensure the semantic is correct");
}

const Pair<const std::string*, size_t>& nemesis::CompileState::GetCurrentRotation() const
{
    if (!CurrentRotationData.empty()) return CurrentRotationData.back();

    throw std::runtime_error("Current rotation data is not registered in queue. Use "
                             "SemanticManager::HasRotationDataInQueue during parsing to "
                             "ensure the semantic is correct");
}

nemesis::CompileState::CompileState(nemesis::CompilationManager& manager) noexcept
    : Manager(manager)
{
}

const nemesis::CompilationManager& nemesis::CompileState::GetManager() const noexcept
{
    return Manager;
}

void nemesis::CompileState::SetBaseRequest(const nemesis::AnimationRequest* request)
{
    BaseRequest = request;
    CurrentRequest.clear();

    auto& parents = request->GetParents();
    auto& name    = request->GetTemplateName();

    QueueCurrentRequest(name + "_" + std::to_string(parents.size() + 1), request);

    for (size_t i = 0; i < parents.size(); ++i)
    {
        QueueCurrentRequest(name + "_" + std::to_string(i + 1), parents[i]);
    }
}

const nemesis::AnimationRequest* nemesis::CompileState::GetBaseRequest() const noexcept
{
    return BaseRequest;
}

const nemesis::AnimationRequest* nemesis::CompileState::GetFirstRequest(const std::string& group) const
{
    return GetChildRequestList(group).front();
}

const nemesis::AnimationRequest* nemesis::CompileState::GetLastRequest(const std::string& group) const
{
    return GetChildRequestList(group).back();
}

const nemesis::AnimationRequest* nemesis::CompileState::GetNextRequest(const std::string& group) const
{
    auto request = GetCurrentRequest(group);
    auto parents = request->GetParents();
    size_t index = request->GetIndex() + 1;

    if (!parents.empty())
    {
        auto requests = parents.back()->GetRequests();

        if (index >= requests.size()) return nullptr;

        return requests[index];
    }

    auto& name     = request->GetTemplateName();
    auto& requests = GetRequests(name);

    if (index >= requests.size()) return nullptr;

    return requests[index];
}

const nemesis::AnimationRequest* nemesis::CompileState::GetBackRequest(const std::string& group) const
{
    auto request = GetCurrentRequest(group);
    size_t index = request->GetIndex() - 1;

    if (index < 0) return nullptr;

    auto parents = request->GetParents();

    if (!parents.empty())
    {
        auto requests = parents.back()->GetRequests();
        return requests[index];
    }

    auto& name     = request->GetTemplateName();
    auto& requests = GetRequests(name);
    return requests[index];
}

void nemesis::CompileState::QueueCurrentRequest(const std::string& group,
                                                const nemesis::AnimationRequest* request)
{
    CurrentRequest[group].emplace_back(request);
    auto requests = request->GetRequests();
    auto parents  = request->GetParents();
    auto& name    = request->GetTemplateName();

    if (parents.empty())
    {
        std::string cur_group = name + "_1";
        auto& collection      = GetRequests(name);
        Vec<const nemesis::AnimationRequest*> anim_requests;
        
        for (auto& each : collection)
        {
            anim_requests.emplace_back(each);
        }

        QueueChildRequestList(cur_group, anim_requests);
    }
    else
    {
        auto parent = parents.back();
        std::string cur_group = name + "_" + std::to_string(parents.size() + 1);
        QueueChildRequestList(cur_group, parent->GetRequests());
    }

    if (requests.empty()) return;

    std::string child_group = name + "_" + std::to_string(parents.size() + 2);
    QueueChildRequestList(child_group, requests);
}

void nemesis::CompileState::DequeCurrentRequest(const std::string& group)
{
    auto request  = CurrentRequest[group].back();
    auto requests = request->GetRequests();
    auto parents  = request->GetParents();
    auto& name    = request->GetTemplateName();

    std::string cur_group = name + "_" + std::to_string(parents.size() + 1);
    DequeChildRequestList(cur_group);

    if (!requests.empty())
    {
        std::string child_group = name + "_" + std::to_string(parents.size() + 2);
        DequeChildRequestList(child_group);
    }

    if (CurrentRequest[group].size() == 1)
    {
        CurrentRequest.erase(group);
        return;
    }

    CurrentRequest[group].pop_back();
}

const nemesis::AnimationRequest* nemesis::CompileState::GetCurrentRequest(const std::string& group) const
{
    auto itr = CurrentRequest.find(group);

    if (itr != CurrentRequest.end()) return itr->second.back();

    throw std::runtime_error("Syntax Error: Request group does not exist in current context");
}

void nemesis::CompileState::QueueChildRequestList(const std::string& group,
                                                  const Vec<const nemesis::AnimationRequest*>& requests)
{
    ChildRequestList[group].emplace_back(requests);
}

void nemesis::CompileState::DequeChildRequestList(const std::string& group)
{
    if (ChildRequestList[group].size() == 1)
    {
        ChildRequestList.erase(group);
        return;
    }

    ChildRequestList[group].pop_back();
}

const Vec<const nemesis::AnimationRequest*>&
nemesis::CompileState::GetChildRequestList(const std::string& group) const
{
    auto itr = ChildRequestList.find(group);

    if (itr != ChildRequestList.end()) return itr->second.back();

    throw std::runtime_error("Syntax Error: Child request group does not exist in current context");
}

void nemesis::CompileState::QueueCurrentRequestMapValue(const nemesis::AnimationRequest* request,
                                                        const std::string& key,
                                                        const std::string& value,
                                                        size_t index)
{
    CurrentRequestMap[request][key].emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentRequestMapValue(const nemesis::AnimationRequest* request,
                                                        const std::string& key)
{
    auto r_itr = CurrentRequestMap.find(request);

    if (r_itr == CurrentRequestMap.end()) return;

    auto itr   = r_itr->second.find(key);

    if (itr == r_itr->second.end()) return;

    if (itr->second.size() != 1)
    {
        itr->second.pop_back();
        return;
    }

    r_itr->second.erase(itr);

    if (!r_itr->second.empty()) return;

    CurrentRequestMap.erase(r_itr);
}

const std::string& nemesis::CompileState::GetCurrentRequestMapValue(const nemesis::AnimationRequest* request,
                                                                    const std::string& key) const
{
    return *GetCurrentRequestMap(request, key).first;
}

size_t nemesis::CompileState::GetCurrentRequestMapIndex(const nemesis::AnimationRequest* request,
                                                        const std::string& key) const
{
    return GetCurrentRequestMap(request, key).second;
}

void nemesis::CompileState::QueueCurrentMapValue(const std::string& key, const std::string& value, size_t index)
{
    CurrentMap[key].emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentMapValue(const std::string& key)
{
    auto itr = CurrentMap.find(key);

    if (itr->second.size() == 1)
    {
        CurrentMap.erase(itr);
        return;
    }

    itr->second.pop_back();
}

const std::string& nemesis::CompileState::GetCurrentMapValue(const std::string& key) const
{
    return *GetCurrentMap(key).first;
}

size_t nemesis::CompileState::GetCurrentMapIndex(const std::string& key) const
{
    return GetCurrentMap(key).second;
}

void nemesis::CompileState::QueueCurrentRequestMotionData(const nemesis::AnimationRequest* request,
                                                          const std::string& value,
                                                          size_t index)
{
    CurrentRequestMotionData[request].emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentRequestMotionData(const nemesis::AnimationRequest* request)
{
    CurrentRequestMotionData.erase(request);
}

const std::string&
nemesis::CompileState::GetCurrentRequestMotionData(const nemesis::AnimationRequest* request) const
{
    return *GetCurrentRequestMotion(request).first;
}

size_t nemesis::CompileState::GetCurrentRequestMotionIndex(const nemesis::AnimationRequest* request) const
{
    return GetCurrentRequestMotion(request).second;
}

void nemesis::CompileState::QueueCurrentMotionData(const std::string& value, size_t index)
{
    CurrentMotionData.emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentMotionData()
{
    CurrentMotionData.pop_back();
}

const std::string& nemesis::CompileState::GetCurrentMotionData() const
{
    return *GetCurrentMotion().first;
}

size_t nemesis::CompileState::GetCurrentMotionIndex() const
{
    return GetCurrentMotion().second;
}

void nemesis::CompileState::QueueCurrentRequestRotationData(const nemesis::AnimationRequest* request,
                                                            const std::string& value,
                                                            size_t index)
{
    CurrentRequestRotationData[request].emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentRequestRotationData(const nemesis::AnimationRequest* request)
{
    CurrentRequestRotationData.erase(request);
}

const std::string&
nemesis::CompileState::GetCurrentRequestRotationData(const nemesis::AnimationRequest* request) const
{
    return *GetCurrentRequestRotation(request).first;
}

size_t nemesis::CompileState::GetCurrentRequestRotationIndex(const nemesis::AnimationRequest* request) const
{
    return GetCurrentRequestRotation(request).second;
}

void nemesis::CompileState::QueueCurrentRotationData(const std::string& value, size_t index)
{
    CurrentRotationData.emplace_back(&value, index);
}

void nemesis::CompileState::DequeCurrentRotationData()
{
    CurrentRotationData.pop_back();
}

const std::string& nemesis::CompileState::GetCurrentRotationData() const
{
    return *GetCurrentRotation().first;
}

size_t nemesis::CompileState::GetCurrentRotationIndex() const
{
    return GetCurrentRotation().second;
}

const nemesis::AnimationRequestCollection&
nemesis::CompileState::GetRequests(const std::string& template_name) const
{
    return Manager.GetAnimationRepository().GetRequests(template_name);
}

void nemesis::CompileState::QueueRequestOption(const nemesis::AnimationRequest* request,
                                               const std::string& option_name,
                                               const nemesis::TemplateOption* option)
{
    CurrentRequestOption[request][option_name].emplace_back(option);
}

void nemesis::CompileState::DequeueRequestOption(const nemesis::AnimationRequest* request,
                                                 const std::string& option_name)
{
    auto r_itr = CurrentRequestOption.find(request);

    if (r_itr == CurrentRequestOption.end()) return;

    auto itr   = r_itr->second.find(option_name);

    if (itr->second.size() != 1)
    {
        itr->second.pop_back();
        return;
    }

    r_itr->second.erase(itr);

    if (!r_itr->second.empty()) return;

    CurrentRequestOption.erase(r_itr);
}

const nemesis::TemplateOption*
nemesis::CompileState::GetCurrentRequestOption(const nemesis::AnimationRequest* request,
                                               const std::string& option_name) const
{
    auto itr = CurrentRequestOption.find(request);

    if (itr == CurrentRequestOption.end()) return nullptr;

    auto k_itr = itr->second.find(option_name);
    return k_itr == itr->second.end() ? nullptr : k_itr->second.back();
}

void nemesis::CompileState::QueueOption(const std::string& option_name, const nemesis::TemplateOption* option)
{
    CurrentOption[option_name].emplace_back(option);
}

void nemesis::CompileState::DequeueOption(const std::string& option_name)
{
    auto itr = CurrentOption.find(option_name);

    if (itr->second.size() == 1)
    {
        CurrentOption.erase(itr);
        return;
    }

    itr->second.pop_back();
}

const nemesis::TemplateOption* nemesis::CompileState::GetCurrentOption(const std::string& option_name) const
{
    auto itr = CurrentOption.find(option_name);
    return itr == CurrentOption.end() ? nullptr : itr->second.back();
}

void* nemesis::CompileState::InsertAddLineHandler(const std::function<void(nemesis::Line&)>& event)
{
    auto func_ptr = std::make_shared<std::function<void(nemesis::Line&, const nemesis::NObject&)>>(
        [event](nemesis::Line& line, const nemesis::NObject& nobject) { event(line); });
    return AddLineEvents.emplace_back(func_ptr).get();
}

void* nemesis::CompileState::InsertAddLineHandler(
    const std::function<void(nemesis::Line&, const nemesis::NObject&)>& event)
{
    auto func_ptr = std::make_shared<std::function<void(nemesis::Line&, const nemesis::NObject&)>>(
        [event](nemesis::Line& line, const nemesis::NObject& nobject) { event(line, nobject); });
    return AddLineEvents.emplace_back(func_ptr).get();
}

void nemesis::CompileState::RemoveAddLineHandler(void* handler_address)
{
    for (size_t i = 0; i < AddLineEvents.size(); ++i)
    {
        if (AddLineEvents[i].get() != handler_address) continue;

        AddLineEvents.erase(AddLineEvents.begin() + i);
        return;
    }

    throw std::runtime_error("Invalid handler address");
}

void nemesis::CompileState::RaiseAddLineEvent(nemesis::Line& line, const nemesis::NObject& nobject) const
{
    Vec<SPtr<std::function<void(nemesis::Line&, const nemesis::NObject&)>>> event_pointers; 

    for (auto& event : AddLineEvents)
    {
        event_pointers.emplace_back(event);
    }

    for (auto& event_ptr : event_pointers)
    {
        (*event_ptr)(line, nobject);
    }
}

std::function<void()>* nemesis::CompileState::InsertEOFHandler(const std::function<void()>& event)
{
    auto func_ptr = std::make_shared<std::function<void()>>([event]() { event(); });
    return EOFEvents.emplace_back(func_ptr).get();
}

void nemesis::CompileState::RemoveEOFHandler(std::function<void()>* handler_address)
{
    for (size_t i = 0; i < EOFEvents.size(); ++i)
    {
        if (EOFEvents[i].get() != handler_address) continue;

        EOFEvents.erase(EOFEvents.begin() + i);
        return;
    }

    throw std::runtime_error("Invalid handler address");
}

void nemesis::CompileState::RaiseEOFEvent() const
{
    Vec<SPtr<std::function<void()>>> event_pointers;

    for (auto& event : EOFEvents)
    {
        event_pointers.emplace_back(event.get());
    }

    for (auto& event_ptr : event_pointers)
    {
        (*event_ptr)();
    }
}

bool nemesis::CompileState::IsModSelected(const std::string& modcode) const
{
    return Manager.IsModSelected(modcode);
}

bool nemesis::CompileState::IsModSelected(const std::string_view& modcode) const
{
    return Manager.IsModSelected(modcode);
}

const VecStr& nemesis::CompileState::GetSelectedMods() const noexcept
{
    return Manager.GetSelectedMods();
}

void nemesis::CompileState::AddSubTemplateRequest(const VecStr& arguments)
{
    if (SubTemplateRequests.find(arguments.front()) != SubTemplateRequests.end()) return;

    SubTemplateRequests.insert_or_assign(arguments.front(), SubTemplateRequest(arguments));
}

void nemesis::CompileState::RemoveSubTemplateRequest(const std::string& expression)
{
    SubTemplateRequests.erase(expression);
}

Vec<const nemesis::SubTemplateRequest*> nemesis::CompileState::GetSubTemplateRequestList() const
{
    Vec<const nemesis::SubTemplateRequest*> requests;

    for (auto& each : SubTemplateRequests)
    {
        requests.emplace_back(&each.second);
    }

    return requests;
}

void nemesis::CompileState::ClearSubTemplateRequestList()
{
    SubTemplateRequests.clear();
}

void nemesis::CompileState::SetCurrentSubTemplateRequest(const nemesis::SubTemplateRequest& request) noexcept
{
    CurrentSubTemplateRequest = &request;
}

const nemesis::SubTemplateRequest* nemesis::CompileState::GetCurrentSubTemplateRequest() const noexcept
{
    return CurrentSubTemplateRequest;
}

const std::string& nemesis::CompileState::GetEventID(const std::string& name) const
{
    auto itr = EventMap.find(name);

    if (itr != EventMap.end()) return itr->second;

    throw std::runtime_error("Event id not found (name: " + name + ")");
}

const std::string& nemesis::CompileState::AddEventName(const std::string& name)
{
    return EventMap[name] = std::to_string(EventMap.size());
}

const std::string& nemesis::CompileState::GetVariableID(const std::string& name)
{
    auto itr = VariableMap.find(name);

    if (itr != VariableMap.end()) return itr->second;

    throw std::runtime_error("Variable id not found (name: " + name + ")");
}

const std::string& nemesis::CompileState::AddVariableName(const std::string& name)
{
    return VariableMap[name] = std::to_string(VariableMap.size());
}

const std::string& nemesis::CompileState::GetAttributeID(const std::string& name) const
{
    auto itr = AttributeMap.find(name);

    if (itr != AttributeMap.end()) return itr->second;

    throw std::runtime_error("Attribute id not found (name: " + name + ")");
}

const std::string& nemesis::CompileState::AddAttributeName(const std::string& name)
{
    return AttributeMap[name] = std::to_string(AttributeMap.size());
}

const std::string& nemesis::CompileState::GetPropertyID(const std::string& name) const
{
    auto itr = PropertyMap.find(name);

    if (itr != PropertyMap.end()) return itr->second;

    throw std::runtime_error("Property id not found (name: " + name + ")");
}

const std::string& nemesis::CompileState::AddPropertyName(const std::string& name)
{
    return PropertyMap[name] = std::to_string(PropertyMap.size());
}

bool nemesis::CompileState::TryGetAnimationOrder(const std::string& name, size_t& value) const
{
    std::scoped_lock lock(AnimationOrderMapMutex);

    auto itr = AnimationOrderMap.find(name);

    if (itr == AnimationOrderMap.end()) return false;

    value = itr->second;
    return true;
}

size_t nemesis::CompileState::GetAnimationOrder(const std::filesystem::path& filepath,
                                                const std::string& name) const
{
    auto compile_state = Manager.GetCompileState(filepath);

    if (!compile_state)
    {
        throw std::runtime_error("Animation order not found (Character: " + filepath.string()
                                 + "Animtion: " + name + ")");
    }

    size_t value;

    if (compile_state->TryGetAnimationOrder(name, value)) return value;

    throw std::runtime_error("Animation order not found (Character: " + filepath.string()
                             + "Animtion: " + name + ")");
}

size_t nemesis::CompileState::AddAnimationToOrder(const std::string& name)
{
    std::scoped_lock lock(AnimationOrderMapMutex);
    return AnimationOrderMap[name] = AnimationOrderMap.size();
}

const std::string& nemesis::CompileState::GetStateID(const std::string& unique_key)
{
    auto itr = StateMap.find(unique_key);

    if (itr != StateMap.end()) return itr->second;

    return StateMap[unique_key] = std::to_string(StateRunningID++);
}

int* nemesis::CompileState::GetCounter(const std::string& name) const
{
    auto itr = CounterMap.find(name);

    if (itr != CounterMap.end()) return itr->second;

    throw std::runtime_error("Element counter name not found (name: " + name + ")");
}

void nemesis::CompileState::AddCounter(const std::string& name, int* counter)
{
    CounterMap[name] = counter;
}

void nemesis::CompileState::RemoveCounter(const std::string& name)
{
    CounterMap.erase(name);
}

void nemesis::CompileState::CacheConditionResult(const std::string& condition_syntax, bool result)
{
    ConditionCache[condition_syntax] = result;
}

Vec<Pair<std::string, bool>> nemesis::CompileState::RemoveConditionCacheContaining(const std::string& syntax)
{
    Vec<Pair<std::string, bool>> removing_list;

    for (auto& each : ConditionCache)
    {
        if (each.first.find(syntax) == NOT_FOUND) continue;

        removing_list.emplace_back(each.first, each.second);
    }

    for (auto& each : removing_list)
    {
        ConditionCache.erase(each.first);
    }

    return removing_list;
}

void nemesis::CompileState::ClearAllConditionCache()
{
    ConditionCache.clear();
}

const bool* nemesis::CompileState::TryGetCacheConditionResult(const std::string& condition_syntax)
{
    auto itr = ConditionCache.find(condition_syntax);

    if (itr != ConditionCache.end()) return &itr->second;

    return nullptr;
}

UPtr<nemesis::CompileState> nemesis::CompileState::Clone()
{
    UPtr<nemesis::CompileState> state = std::make_unique<nemesis::CompileState>(Manager);
    state->BaseRequest      = BaseRequest;
    state->CurrentRequest   = CurrentRequest;
    state->ChildRequestList = ChildRequestList;

    state->CurrentRequestOption = CurrentRequestOption;
    state->CurrentOption        = CurrentOption;

    state->CurrentRequestMap = CurrentRequestMap;
    state->CurrentMap        = CurrentMap;

    state->CurrentRequestMotionData = CurrentRequestMotionData;
    state->CurrentMotionData        = CurrentMotionData;

    state->CurrentRequestRotationData = CurrentRequestRotationData;
    state->CurrentRotationData        = CurrentRotationData;

    state->AddLineEvents = AddLineEvents;
    state->EOFEvents     = EOFEvents;

    state->SubTemplateRequests = SubTemplateRequests;

    state->CurrentSubTemplateRequest = CurrentSubTemplateRequest;

    state->EventMap          = EventMap;
    state->VariableMap       = VariableMap;
    state->AttributeMap      = AttributeMap;
    state->PropertyMap       = PropertyMap;
    state->StateMap          = StateMap;
    state->AnimationOrderMap = AnimationOrderMap;

    state->CounterMap = CounterMap;

    state->ConditionCache = ConditionCache;
    return state;
}
