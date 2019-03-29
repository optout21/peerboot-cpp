#pragma once
#include "ipebo_net.hpp"
#include "ipebo_peer.hpp"
#include "store.hpp"
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
            std::string id;
            std::shared_ptr<IPeboPeer> peer;
        };
        PeboNet(IStore* store_in);
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        errorCode init(std::string id_in);
        errorCode deinit();
        errorCode addPeer(std::string id_in, std::shared_ptr<IPeboPeer> const & peer_in);
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode query(service_t service_in);
        void notifyFromPeboPeer(std::string id_in, PeerInfo peer_in);
        void queryFromPeboPeer(std::string id_in, service_t service_in);

    private:
        errorCode doBroadcast(PeerInfo const & peer_in, std::string originatorPeer);
        errorCode doQuery(std::string id_in, service_t service_in);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        std::string myId;
        IPeboNetCB* myPeboNetCB;
        std::vector<PeerWithId> myNetPeers;
        IStore* myStore;
    };
}