#pragma once

#include <string>

#include "scope/scopeinfo.h"

namespace nemesis
{
	struct AnimValue
    {
        virtual std::string GetResult(nemesis::ScopeInfo& scopeinfo) const = 0;
    };
}
