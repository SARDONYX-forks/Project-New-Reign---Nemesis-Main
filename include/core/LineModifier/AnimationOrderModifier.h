#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/AnimationOrderStatement.h"

namespace nemesis
{
    struct AnimationOrderModifier : public nemesis::StandardLineModifier<nemesis::AnimationOrderStatement>
    {
    public:
        AnimationOrderModifier(size_t begin,
                               size_t end,
                               const std::string& expression,
                               size_t linenum,
                               const std::filesystem::path& filepath,
                               const nemesis::SemanticManager& manager);
    };
}
