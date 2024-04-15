#pragma once

#include "core/LineModifier/StandardLineModifier.h"

#include "core/Statements/RotationDataStatement.h"

namespace nemesis
{
    struct SemanticManager;

    struct RotationDataModifier : public nemesis::StandardLineModifier<nemesis::RotationDataStatement>
    {
    public:
        RotationDataModifier(size_t begin,
                             size_t end,
                             const std::string& expression,
                             size_t linenum,
                             const std::filesystem::path& filepath,
                             const nemesis::SemanticManager& manager);
    };
}
