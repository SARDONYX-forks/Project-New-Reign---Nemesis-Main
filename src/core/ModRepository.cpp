#include "core/ModRepository.h"

nemesis::ModRepository::ModRepository(const std::filesystem::path& mods_dir_path)
{
    for (auto& entry : std::filesystem::directory_iterator(mods_dir_path))
    {
        if (!entry.is_directory()) continue;

        auto ini_path = entry.path() / "info.ini";

        if (!std::filesystem::exists(ini_path)) continue;

        ModClassList.emplace_back(std::make_unique<nemesis::ModClass>(ini_path));
    }
}

Vec<const nemesis::ModClass*> nemesis::ModRepository::GetModClassList() const
{
    if (ModClassListCache) return *ModClassListCache;

    ModClassListCache = std::make_unique<Vec<const nemesis::ModClass*>>();

    for (auto& mod_class : ModClassList)
    {
        ModClassListCache->emplace_back(mod_class.get());
    }

    return *ModClassListCache;
}
