#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/MapStatement.h"

namespace nemesis
{
    struct SemanticManager;

	struct MapModifier : public nemesis::StandardLineModifier<nemesis::MapStatement>
    {
    public:
        MapModifier(size_t begin,
                    size_t end,
                    const std::string& expression,
                    size_t linenum,
                    const std::filesystem::path& filepath,
                    const nemesis::SemanticManager& manager);
    };
}
