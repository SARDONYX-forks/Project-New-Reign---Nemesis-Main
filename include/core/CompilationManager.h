#pragma once

#include <mutex>

#include "core/Template.h"
#include "core/CompileState.h"

namespace nemesis
{
	struct CompilationManager
    {
    private:
        const nemesis::AnimationRequestRepository& AnimationRepository;
        const nemesis::TemplateRepository& TemplateRepository;
        UMap<std::string, nemesis::CompileState> StateList;
        VecStr SelectedMods;

        std::mutex CreateMutex;

    public:
        CompilationManager(const VecStr& selected_mods,
                           const nemesis::AnimationRequestRepository& anim_repo,
                           const nemesis::TemplateRepository& templt_repo);

        bool IsModSelected(const std::string& modcode) const;
        bool IsModSelected(const std::string_view& modcode) const;
        const VecStr& GetSelectedMods() const noexcept;

        nemesis::CompileState& CreateCompileState(const std::filesystem::path& filepath);
        nemesis::CompileState* GetCompileState(const std::filesystem::path& filepath);
        const nemesis::AnimationRequestRepository& GetAnimationRepository() const noexcept;
        const nemesis::TemplateRepository& GetTemplateRepository() const noexcept;
    };
}
