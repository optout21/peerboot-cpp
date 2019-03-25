#pragma once
#include "ipebo_net.hpp"
#include "ipebo_peer.hpp"
#include <memory>
#include <vector>

namespace pebo
{
    /**
     * Represents the PeerBoot network.
     * TODO: fill it
     */
    class PeboNet : public IPeboNet
    {
    public:
        PeboNet() = default;
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        errorCode init();
        errorCode deinit();
        errorCode addPeer(std::shared_ptr<IPeboPeer> const & peer_in);
        errorCode broadcast(PeerInfo const & peer_in);

    private:
        errorCode doBroadcast(PeerInfo const & peer_in);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        IPeboNetCB* myPeboNetCB;
        std::vector<std::shared_ptr<IPeboPeer>> myNetPeers;
    };
}