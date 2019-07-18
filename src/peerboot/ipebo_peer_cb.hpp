#pragma once
#include "../include/peerboot.hpp"
#include "message.hpp"

namespace pebo
{
    class IPeboPeer; // forward

    /**
     * Callback interface from PeboPeer.
     */
    class IPeboPeerCB
    {
    public:
        // /// Called when an incoming connection has finished
        //virtual void connectionClosed(IPeboPeer* client_in) = 0;
        // A message received from the peer
        virtual void msgFromPeboPeer(IPeboPeer& peer_in, BaseMessage const & msg_in) = 0;
    };
}