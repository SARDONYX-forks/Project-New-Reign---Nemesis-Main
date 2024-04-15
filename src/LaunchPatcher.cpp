#include "LaunchPatcher.h"
#include "nemesisinfo.h"

#include "utilities/algorithm.h"

#include "core/CoreObject.h"

namespace sf = std::filesystem;

extern sf::path CurrentExeDirectory;

void nemesis::LaunchPatcher(const VecStr& mod_selected)
{
    auto* nemesis_info   = NemesisInfo::GetInstance();
    nemesis::NObjectRepository repo(nemesis_info->GetDataPath());
    nemesis::TemplateRepository templt_repo(CurrentExeDirectory / L"behavior_templates", repo);
    nemesis::AnimationRequestRepository anim_repo(nemesis_info->GetDataPath(), templt_repo);
    nemesis::ModRepository mod_repo(CurrentExeDirectory / "mods");
    repo.Patch(mod_repo);

    nemesis::CompilationManager manager({"nemesis"}, anim_repo, templt_repo);
    repo.Compile(manager);
}
