#pragma once

#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct ParsableObject
    {
        virtual void AppendLine(const nemesis::Line& line) = 0;
    };
} // namespace nemesis
