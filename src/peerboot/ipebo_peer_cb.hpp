#pragma once
#include "../include/peerboot.hpp"
#include "message.hpp"

namespace pebo
{
    /**
     * Callback interface from PeboPeer.
     */
    class IPeboPeerCB
    {
    public:
        // A message received from the peer
        virtual void msgFromPeboPeer(std::string nodeId_in, BaseMessage const & msg_in) = 0;
    };
}