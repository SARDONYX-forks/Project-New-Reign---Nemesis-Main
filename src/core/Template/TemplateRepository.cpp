#include "core/Template/TemplateRepository.h"

nemesis::TemplateRepository::TemplateRepository(const std::filesystem::path& templt_dir,
                                                nemesis::NObjectRepository& repo)
{
    for (auto& entry : std::filesystem::directory_iterator(templt_dir))
    {
        auto path = entry.path();

        if (!entry.is_directory())
        {
            if (!nemesis::iequals(path.extension().wstring(), L".txt")) continue;

            auto st_obj = nemesis::SubTemplateObject::ParseFromFile(path);
            SubTemplateList.emplace_back(std::move(st_obj));
            continue;
        }

        auto temp_class = nemesis::TemplateClass::ParseTemplateClassFromDirectory(path, repo);

        if (!temp_class) continue;

        TemplateClassList.emplace_back(std::move(temp_class));
    }
}

UPtr<nemesis::AnimationRequest> nemesis::TemplateRepository::CreateRequest(
    const std::string& request_info, size_t linenum, const std::filesystem::path& filepath)
{
    std::string templt_code;

    for (auto& ch : request_info)
    {
        if (ch == ' ') break;

        templt_code.push_back(ch);
    }

    for (auto& templt_class : TemplateClassList)
    {
        if (!nemesis::iequals(templt_class->GetName(), templt_code)) continue;

        return templt_class->CreateRequest(request_info, linenum, filepath);
    }

    return nullptr;
}

Vec<const nemesis::TemplateClass*> nemesis::TemplateRepository::GetTemplateClassList() const noexcept
{
    if (TemplateClassList_Cache) return *TemplateClassList_Cache;

    TemplateClassList_Cache = std::make_unique<Vec<const nemesis::TemplateClass*>>();

    for (auto& templt_class : TemplateClassList)
    {
        TemplateClassList_Cache->emplace_back(templt_class.get());
    }
    
    return *TemplateClassList_Cache;
}

Vec<const nemesis::SubTemplateObject*> nemesis::TemplateRepository::GetSubTemplateList() const noexcept
{
    if (SubTemplateList_Cache) return *SubTemplateList_Cache;

    SubTemplateList_Cache = std::make_unique<Vec<const nemesis::SubTemplateObject*>>();

    for (auto& sub_templt : SubTemplateList)
    {
        SubTemplateList_Cache->emplace_back(sub_templt.get());
    }

    return *SubTemplateList_Cache;
}
