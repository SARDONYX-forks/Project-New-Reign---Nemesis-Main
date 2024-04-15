#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/AnimationFilePathStatement.h"

namespace nemesis
{
    struct AnimationFilePathModifier
        : public nemesis::StandardLineModifier<nemesis::AnimationFilePathStatement>
    {
    public:
        AnimationFilePathModifier(size_t begin,
                                  size_t end,
                                  const std::string& expression,
                                  size_t linenum,
                                  const std::filesystem::path& filepath,
                                  const nemesis::SemanticManager& manager);
    };
}
