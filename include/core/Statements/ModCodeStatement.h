#pragma once

#include "core/Statements/ConditionalStatement.h"
#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct CompileState;

    struct ModCodeStatement : public nemesis::ConditionalStatement
    {
    public:
        ModCodeStatement(const std::string& mod_code,
                         size_t linenum,
                         const std::filesystem::path& filepath,
                         nemesis::SemanticManager& manager) noexcept;
        ModCodeStatement(const nemesis::Line& mod_code, nemesis::SemanticManager& manager) noexcept;

        std::string Serialize() const override;

        bool IsTrue(nemesis::CompileState& state) const override;
    };
} // namespace nemesis
