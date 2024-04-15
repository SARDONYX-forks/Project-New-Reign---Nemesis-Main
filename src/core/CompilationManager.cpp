#include "core/CompilationManager.h"

nemesis::CompilationManager::CompilationManager(const VecStr& selected_mods,
                                                const nemesis::AnimationRequestRepository& anim_repo,
                                                const nemesis::TemplateRepository& templt_repo)
    : SelectedMods(selected_mods)
    , AnimationRepository(anim_repo)
    , TemplateRepository(templt_repo)
{
}

bool nemesis::CompilationManager::IsModSelected(const std::string& modcode) const
{
    for (size_t i = 0; i < SelectedMods.size(); ++i)
    {
        if (modcode == SelectedMods[i]) return true;
    }

    return false;
}

bool nemesis::CompilationManager::IsModSelected(const std::string_view& modcode) const
{
    for (size_t i = 0; i < SelectedMods.size(); ++i)
    {
        if (modcode == SelectedMods[i]) return true;
    }

    return false;
}

const VecStr& nemesis::CompilationManager::GetSelectedMods() const noexcept
{
    return SelectedMods;
}

nemesis::CompileState& nemesis::CompilationManager::CreateCompileState(const std::filesystem::path& filepath)
{
    std::scoped_lock lock(CreateMutex);

    auto itr = StateList.find(filepath.string());

    if (itr == StateList.end()) return StateList.try_emplace(filepath.string(), *this).first->second;

    throw std::runtime_error("CompileState for file already exist (" + filepath.string() + ")");
}

nemesis::CompileState* nemesis::CompilationManager::GetCompileState(const std::filesystem::path& filepath)
{
    auto itr = StateList.find(filepath.string());

    if (itr != StateList.end()) return &itr->second;

    return nullptr;
}

const nemesis::AnimationRequestRepository& nemesis::CompilationManager::GetAnimationRepository() const noexcept
{
    return AnimationRepository;
}

const nemesis::TemplateRepository& nemesis::CompilationManager::GetTemplateRepository() const noexcept
{
    return TemplateRepository;
}
