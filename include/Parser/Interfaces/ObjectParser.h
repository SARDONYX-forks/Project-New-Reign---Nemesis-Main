#pragma once

#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct ObjectParser
    {
        virtual void ParseLine(const nemesis::Line& line) = 0;
    };
} // namespace nemesis
