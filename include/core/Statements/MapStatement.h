#pragma once

#include <functional>

#include "core/Statements/CompositeStatement.h"

namespace nemesis
{
    struct CompileState;
    struct SemanticManager;

	struct MapStatement : public nemesis::CompositeStatement
    {
    private:
        std::function<std::string(nemesis::CompileState&)> GetValueFunction;

        bool TryParse3Components(const nemesis::SemanticManager& manager);
        bool TryParse5Components(const nemesis::SemanticManager& manager);

    public:
        MapStatement(const std::string& expression,
                     size_t linenum,
                     const std::filesystem::path& filepath,
                     const nemesis::SemanticManager& manager);

        std::string GetValue(nemesis::CompileState& state) const;
    };
}
