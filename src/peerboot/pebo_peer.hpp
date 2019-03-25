#pragma once
#include "ipebo_peer.hpp"

namespace pebo
{
    /**
     * Represents a PeerBoot peer.
     */
    class PeboPeer: public IPeboPeer
    {
    public:
        // Send a peer info towards this peer
        errorCode send(PeerInfo const & peer_in);
    };
}