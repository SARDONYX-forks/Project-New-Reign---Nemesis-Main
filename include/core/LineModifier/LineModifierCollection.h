#pragma once

#include "core/LineModifier/LineModifier.h"
#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct LineModifierCollection
    {
    private:
        Vec<UPtr<nemesis::LineModifier>> Modifiers;

    public:
        void AddModifier(UPtr<nemesis::LineModifier> modifier);
        nemesis::Line Apply(const nemesis::Line& line, nemesis::CompileState& state);
    };
} // namespace nemesis
