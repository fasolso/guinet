#pragma once

#include <queue>

namespace net
{
    template<class PollerEvent>
    class PollerBase
    {
    public:
        virtual ~PollerBase() {}

        virtual int poll(int maxEvents) = 0;
    private:
        std::queue<PollerEvent> m_events;
    };
}