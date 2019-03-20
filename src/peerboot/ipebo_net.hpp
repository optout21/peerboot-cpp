#pragma once
#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Represents the PeerBoot network.
     */
    class IPeboNet
    {
    public:
        // Broadcast peer info towards the network
        virtual errorCode broadcast(peer_t peer_in) = 0;
    };
}