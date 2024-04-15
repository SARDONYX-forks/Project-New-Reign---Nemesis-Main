#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/RunningNumberStatement.h"

namespace nemesis
{
    struct RunningNumberModifier : public nemesis::StandardLineModifier<nemesis::RunningNumberStatement>
    {
    public:
        RunningNumberModifier(size_t begin,
                              size_t end,
                              const std::string& expression,
                              size_t linenum,
                              const std::filesystem::path& filepath,
                              const nemesis::SemanticManager& manager);
    };
}
