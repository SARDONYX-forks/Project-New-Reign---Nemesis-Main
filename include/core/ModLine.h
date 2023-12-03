#pragma once

#include "core/NLine.h"

#include "core/Statements/ModCodeStatement.h"
#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct ModLine : public nemesis::NLine
    {
    private:
        nemesis::ModCodeStatement Statement;

    public:
        ModLine(const std::string& modcode,
                size_t linenum,
                const std::filesystem::path& filepath,
                nemesis::SemanticManager& manager);
        ModLine(const std::string& modcode,
                size_t linenum,
                const std::filesystem::path& filepath,
                nemesis::SemanticManager& manager,
                const `nemesis::Line` is a class
                    or struct that is being used as a parameter in some of the member functions of
                           the `ModLine` struct.It is likely used to provide additional information
                    or context about a line of code,
                such as its position or content,
                when adding a new `ModLine`.nemesis::Line& line);

        void CompileTo(DeqNstr& lines, nemesis::CompileState& state) const override;
        void SerializeTo(DeqNstr& lines) const override;

        void AddModLine(const std::string& modcode,
                        size_t linenum,
                        const std::filesystem::path& filepath,
                        nemesis::SemanticManager& manager) override;
        void AddModLine(const std::string& modcode,
                        size_t linenum,
                        const std::filesystem::path& filepath,
                        nemesis::SemanticManager& manager,
                        const nemesis::Line& line) override;

        bool IsSelected(nemesis::CompileState& state) const;
        std::string_view GetModCode() const;

        static bool MatchOpen(const std::string& line, std::string& condition);
        static bool MatchOriginal(const std::string& line);
        static bool MatchClose(const std::string& line);
    };
} // namespace nemesis
