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
        struct PeerWithId
        {
        public:
            long id;
            std::shared_ptr<IPeboPeer> peer;
        };
        PeboNet();
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        errorCode init();
        errorCode deinit();
        errorCode addPeer(long id_in, std::shared_ptr<IPeboPeer> const & peer_in);
        errorCode broadcast(PeerInfo const & peer_in);
        void notifyFromPeboPeer(long id_in, PeerInfo peer_in);

    private:
        errorCode doBroadcast(PeerInfo const & peer_in, long originatorPeer);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        IPeboNetCB* myPeboNetCB;
        std::vector<PeerWithId> myNetPeers;
    };
}