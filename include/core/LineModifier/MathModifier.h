#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/MathStatement.h"

namespace nemesis
{
    struct MathModifier : public nemesis::StandardLineModifier<nemesis::MathStatement>
    {
    public:
        MathModifier(size_t begin,
                     size_t end,
                     const std::string& expression,
                     size_t linenum,
                     const std::filesystem::path& filepath,
                     const nemesis::SemanticManager& manager);

    };
}
