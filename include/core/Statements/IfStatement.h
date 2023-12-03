#pragma once

#include "core/Statements/ConditionalStatement.h"
#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct CompileState;
    struct Template;

    struct IfStatement : public nemesis::ConditionalStatement
    {
    protected:
        bool Negative = false;

    public:
        IfStatement(const std::string& expression,
                    size_t linenum,
                    const std::filesystem::path& filepath,
                    const nemesis::SemanticManager& manager);
        IfStatement(const nemesis::Line& line, const nemesis::SemanticManager& manager);

        std::string Serialize() const override;

        bool IsTrue(nemesis::CompileState& state) const override;
    };
} // namespace nemesis
