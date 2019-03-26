#pragma once
#include "../include/peerboot.hpp"
#include "ipebo_peer_cb.hpp"
#include "ipebo_peer.hpp"
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
    class IPeboNet : public IPeboPeerCB
    {
    public:
        virtual void setNotifyCB(IPeboNetCB* peboNetCB_in) = 0;
        virtual errorCode init() = 0;
        virtual errorCode deinit() = 0;
        virtual errorCode addPeer(std::string id_in, std::shared_ptr<IPeboPeer> const & peer_in) = 0;
        // Broadcast peer info towards the network
        virtual errorCode broadcast(PeerInfo const & peer_in) = 0;
    };
}