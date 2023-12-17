#include "core/NLine.h"
#include "core/ModLine.h"
#include "core/condition.h"
#include "core/CompileState.h"

#include "utilities/conditionsyntax.h"

namespace ns = nemesis::syntax;

nemesis::NLine::NLine(const std::string& expression,
                      size_t linenum,
                      const std::filesystem::path& filepath,
                      const nemesis::SemanticManager& manager)
{
    Value     = std::make_unique<nemesis::Line>(expression, linenum, filepath);
    Modifiers = LineModifierFactory::BuildModifiers(expression, linenum, filepath, manager);
}

nemesis::NLine::NLine(const nemesis::Line& line, const nemesis::SemanticManager& manager)
{
    Value = std::make_unique<nemesis::Line>(line, line.GetLineNumber(), line.GetFilePath());
    Modifiers = LineModifierFactory::BuildModifiers(line, line.GetLineNumber(), line.GetFilePath(), manager);
}

void nemesis::NLine::CompileTo(DeqNstr& lines, nemesis::CompileState& state) const
{
    if (!ModLines.empty())
    {
        auto mods = state.GetSelectedMods();

        for (auto& mod : mods)
        {
            auto modline = ModLines.find(mod);

            if (modline == ModLines.end()) continue;

            modline->second->CompileTo(lines, state);
            return;
        }
    }

    if (!Value || Value->find(ns::DeleteLine()) != NOT_FOUND) return;

    if (Modifiers.empty())
    {
        auto& line = lines.emplace_back(*Value);
        state.RaiseAddLineEvent(line);
        return;
    }

    VecStr tokens;
    std::string merged;

    for (size_t i = 0; i < Value->length(); i++)
    {
        tokens.emplace_back(1, (*Value)[i]);
    }

    for (auto& modifier_list : Modifiers)
    {
        for (auto& mod : modifier_list.second)
        {
            mod->Apply(tokens, state);
        }
    }

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        merged.append(tokens[i]);
    }

    auto& line_ref = lines.emplace_back(merged);
    state.RaiseAddLineEvent(line_ref);
}

void nemesis::NLine::SerializeTo(DeqNstr& lines) const
{
    nemesis::Line value = Value ? *Value : nemesis::Line(ns::DeleteLine());

    if (ModLines.empty())
    {
        lines.emplace_back(value);
        return;
    }

    for (auto& modline : ModLines)
    {
        modline.second->SerializeTo(lines);
    }

    lines.emplace_back(value + ns::Spaces() + ns::LowerOriginal());
}

void nemesis::NLine::MatchAndUpdate(nemesis::NLine& nline)
{
    if (!nline.Value || *Value != *nline.Value)
    {
        throw std::runtime_error("Template update line to line to does not match");
    }

    if (nline.ModLines.empty()) return;

    for (auto& mod_line : nline.ModLines)
    {
        ModLines[mod_line.first] = std::move(mod_line.second);
    }
}

void nemesis::NLine::AddModLine(const std::string& modcode,
                                size_t linenum,
                                const std::filesystem::path& filepath,
                                nemesis::SemanticManager& manager)
{
    ModLines.emplace(modcode, std::make_unique<nemesis::ModLine>(modcode, linenum, filepath, manager));
}

void nemesis::NLine::AddModLine(const std::string& modcode,
                                size_t linenum,
                                const std::filesystem::path& filepath,
                                nemesis::SemanticManager& manager,
                                const nemesis::Line& line)
{
    ModLines.emplace(modcode, std::make_unique<nemesis::ModLine>(modcode, linenum, filepath, manager, line));
}
