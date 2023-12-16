#pragma once

#include <string>

namespace nemesis
{
    struct ScopeInfo;

    struct AnimValue
    {
        virtual std::string GetResult(ScopeInfo& scopeinfo) const = 0;
    };
}
