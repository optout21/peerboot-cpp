#pragma once
#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Callback interface from PeboNet.
     */
    class IPeboNetCB
    {
    public:
        virtual void notifyFromPeboNet(peer_t peer_in) = 0;
    };

    /**
     * Represents the PeerBoot network.
     */
    class IPeboNet
    {
    public:
        virtual void setNotifyCB(IPeboNetCB* peboNetCB_in) = 0;
        virtual errorCode init() = 0;
        virtual errorCode deinit() = 0;
        // Broadcast peer info towards the network
        virtual errorCode broadcast(peer_t peer_in) = 0;
    };
}