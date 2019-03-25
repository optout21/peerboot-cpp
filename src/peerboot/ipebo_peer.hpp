#pragma once
#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Represents a PeerBoot peer.
     */
    class IPeboPeer
    {
    public:
        // Send a peer info towards this peer
        virtual errorCode send(PeerInfo const & peer_in) = 0;
    };
}