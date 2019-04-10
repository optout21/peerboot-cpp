#pragma once
#include "../include/peerboot.hpp"
//#include "ipebo_peer_cb.hpp"
#include "message.hpp"

namespace pebo
{
    class IPeboPeerCB; // forward

    /**
     * Represents a PeerBoot peer.
     */
    class IPeboPeer
    {
    public:
        virtual void setNotifyCB(IPeboPeerCB* peboPeerCB_in) = 0;
        // Send a message to this peer
        virtual errorCode sendMsg(BaseMessage const & msg_in) = 0;
    };
}