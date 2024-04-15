#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/RequestIdStatement.h"

namespace nemesis
{
    struct RequestIdModifier : public nemesis::StandardLineModifier<nemesis::RequestIdStatement>
    {
    public:
        RequestIdModifier(size_t begin,
                          size_t end,
                          const std::string& expression,
                          size_t linenum,
                          const std::filesystem::path& filepath,
                          const nemesis::SemanticManager& manager);
    };
}
