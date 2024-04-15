#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/LengthStatement.h"

namespace nemesis
{
	struct LengthModifier : public nemesis::StandardLineModifier<nemesis::LengthStatement>
    {
    public:
        LengthModifier(size_t begin,
                       size_t end,
                       const std::string& expression,
                       size_t linenum,
                       const std::filesystem::path& filepath,
                       const nemesis::SemanticManager& manager);
    };
}
