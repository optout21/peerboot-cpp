#pragma once
#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Callback interface from PeboPeer.
     */
    class IPeboPeerCB
    {
    public:
        virtual void notifyFromPeboPeer(long id_in, PeerInfo peer_in) = 0;
    };
}