#pragma once

#include "utilities/types.h"

#include "core/AnimationRequest.h"

namespace nemesis
{
    struct AnimationRequestCollection
    {
    private:
        Vec<UPtr<nemesis::AnimationRequest>> uRequestList;
        Vec<const nemesis::AnimationRequest*> RequestList;

    public:
        template <class... _Valty>
        _CONSTEXPR20_CONTAINER decltype(auto) emplace_back(_Valty&&... _Val)
        {
            return RequestList.emplace_back(uRequestList.emplace_back(_STD forward<_Valty>(_Val)...).get());
        }

        const nemesis::AnimationRequest* operator[](size_t index) const noexcept
        {
            return RequestList[index];
        }

        const nemesis::AnimationRequest* at(size_t index) const
        {
            return RequestList.at(index);
        }

        std::vector<const nemesis::AnimationRequest*>::const_iterator begin() const noexcept
        {
            return RequestList.begin();
        }
        
        std::vector<const nemesis::AnimationRequest*>::const_iterator end() const noexcept
        {
            return RequestList.end();
        }

        bool empty() const noexcept
        {
            return RequestList.empty();
        }

        size_t size() const noexcept
        {
            return RequestList.size();
        }
    };
}
