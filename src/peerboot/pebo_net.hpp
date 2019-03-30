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
            std::string nodeId;
            std::shared_ptr<IPeboPeer> peer;
        };
        PeboNet(IStore* store_in);
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        errorCode init(std::string nodeId_in);
        errorCode deinit();
        errorCode addPeer(std::string nodeId_in, std::shared_ptr<IPeboPeer> const & peer_in);
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode queryRemote(service_t service_in);
        std::string getNodeId() { return myNodeId; };
        void notifyFromPeboPeer(std::string nodeId_in, PeerInfo peer_in);
        void queryFromPeboPeer(std::string nodeId_in, service_t service_in);

    private:
        errorCode doBroadcast(PeerInfo const & peer_in, std::string originatorNode);
        errorCode doBroadcastQuery(service_t service_in, std::string originatorNode);
        errorCode doQuery(std::string nodeId_in, service_t service_in);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        std::string myNodeId;
        IPeboNetCB* myPeboNetCB;
        std::vector<PeerWithId> myNetPeers;
        IStore* myStore;
    };
}