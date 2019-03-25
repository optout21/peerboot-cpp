#pragma once
#include "../include/peerboot.hpp"
#include <memory>

namespace pebo
{
    /**
     * Callback interface from PeboNet.
     */
    class IPeboNetCB
    {
    public:
        virtual void notifyFromPeboNet(PeerInfo peer_in) = 0;
    };

    /**
     * Represents the PeerBoot network.
     */
    class IPeboNet
    {
    public:
        virtual void setNotifyCB(std::shared_ptr<IPeboNetCB> peboNetCB_in) = 0;
        virtual errorCode init() = 0;
        virtual errorCode deinit() = 0;
        // Broadcast peer info towards the network
        virtual errorCode broadcast(PeerInfo peer_in) = 0;
    };
}