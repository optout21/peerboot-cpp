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
        virtual void notifyFromPeboPeer(std::string id_in, PeerInfo peer_in) = 0;
    };
}