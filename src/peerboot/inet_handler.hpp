#pragma once

#include "../include/peerboot.hpp"

#include <memory>

namespace pebo
{
    class IPeboNet; // forward

    /**
     * Network connection handler, listening, connecting.
     */
    class INetHandler
    {
    public:
        virtual void init(std::shared_ptr<IPeboNet> peboNet_in) = 0;
        virtual errorCode start(int port_in, int tryNextPorts_in) = 0;
        virtual errorCode stop() = 0;
    };
}
