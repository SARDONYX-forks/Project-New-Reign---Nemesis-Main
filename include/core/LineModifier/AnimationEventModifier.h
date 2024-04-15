#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/AnimationEventStatement.h"

namespace nemesis
{
    struct AnimationEventModifier : public nemesis::StandardLineModifier<nemesis::AnimationEventStatement>
    {
    public:
        AnimationEventModifier(size_t begin,
                               size_t end,
                               const std::string& expression,
                               size_t linenum,
                               const std::filesystem::path& filepath,
                               const nemesis::SemanticManager& manager);
    };
}
