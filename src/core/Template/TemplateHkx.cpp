#include "core/Template/TemplateHkx.h"
#include "core/Template/TemplateClass.h"

#include "core/LineStream.h"
#include "core/NObjectParser.h"

#include "utilities/stringextension.h"

nemesis::TemplateHkx::TemplateHkx(const nemesis::TemplateClass* template_class) noexcept
    : nemesis::TemplateObject(template_class)
{
}

SPtr<nemesis::TemplateHkx> nemesis::TemplateHkx::ParseFromFile(const nemesis::TemplateClass* template_class,
                                                               const std::filesystem::path& filepath)
{
    std::string name(template_class->GetName());

    if (std::find_if(name.begin(), name.end(), [](const char& ch) { return !std::isalpha(ch); })
        != name.end())
    {
        throw std::runtime_error("Invalid template name (" + name + ")");
    }

    std::string filename = filepath.stem().string();
    nemesis::regex name_rgx("^(" + name + ")_([0-9]+)$");
    nemesis::smatch match;

    if (!nemesis::regex_match(filename, match, name_rgx))
    {
        throw std::runtime_error("Invalid filename "
                                 "(Template: "
                                 + name + ", File: " + filepath.string() + ")");
    }

    return NewCustomTemplateObject<nemesis::TemplateHkx>(template_class, std::stoul(match[2]), filepath);
}

SPtr<nemesis::TemplateHkx> nemesis::TemplateHkx::ParseFromFile(const nemesis::TemplateClass* template_class,
                                                                const std::filesystem::path& filepath,
                                                                nemesis::ThreadPool& thread_pool)
{
    std::string name(template_class->GetName());

    if (std::find_if(name.begin(), name.end(), [](const char& ch) { return !std::isalpha(ch); })
        != name.end())
    {
        throw std::runtime_error("Invalid template name (" + name + ")");
    }

    std::string filename = filepath.stem().string();
    nemesis::regex name_rgx("^(" + name + ")_([0-9]+)$");
    nemesis::smatch match;

    if (!nemesis::regex_match(filename, match, name_rgx))
    {
        throw std::runtime_error("Invalid filename "
                                 "(Template: "
                                 + name + ", File: " + filepath.string() + ")");
    }

    return NewCustomTemplateObject<nemesis::TemplateHkx>(template_class, std::stoul(match[2]), filepath, thread_pool);
}
