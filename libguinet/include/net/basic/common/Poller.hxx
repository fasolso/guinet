#pragma once

#include <queue>

namespace net
{
    template<class PollerEvent>
    class PollerBase
    {
    public:
        virtual ~PollerBase() {}

        int poll(int maxEvents);
    private:
        std::queue<PollerEvent> m_events;
    };
}