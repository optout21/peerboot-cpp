#pragma once
#include "ipebo_net.hpp"
#include "ipebo_peer.hpp"
#include "message.hpp"
//#include "inet_handler.hpp"
#include "store.hpp"

#include <memory>
#include <mutex>
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
        //errorCode init();
        //errorCode deinit();
        errorCode start();
        errorCode stop();
        errorCode addPeer(std::string nodeId_in, std::shared_ptr<IPeboPeer> const & peer_in);
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode queryRemote(service_t service_in);
        //std::string getNodeId() { return myNodeId; }
        //void setNodeId(std::string const & nodeId_in);
        // A message received from the peer
        void msgFromPeboPeer(std::string nodeId_in, BaseMessage const & msg_in);
        static const std::string myLoalNodeId;

    private:
        void peerUpdateFromPeboPeer(std::string nodeId_in, PeerUpdateMessage const & msg_in);
        void queryFromPeboPeer(std::string nodeId_in, QueryMessage const & msg_in);
        // origPrevHopNodeId_in: the nodeId of the node where this came from, to avoid sending back; or myLoalNodeId for locally-originated messages.
        errorCode doBroadcastMsg(BaseMessage const & msg_in, std::string origPrevHopNodeId_in);
        errorCode doQuery(std::string nodeId_in, service_t service_in);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        //std::string myNodeId;
        IPeboNetCB* myPeboNetCB;
        std::vector<PeerWithId> myNetPeers;
        std::recursive_mutex myNetPeersMutex;
        IStore* myStore;

        // Put as inner class to access private methods
        class FromPeerMessageVisitor: public MessageVisitorBase
        {
        public:
            FromPeerMessageVisitor(PeboNet & net_in, std::string nodeId_in) :
                MessageVisitorBase(), myNet(net_in), myNodeId(nodeId_in) { }
            virtual ~FromPeerMessageVisitor() = default;
            void peerUpdate(PeerUpdateMessage const & msg_in) { myNet.peerUpdateFromPeboPeer(myNodeId, msg_in); }
            void query(QueryMessage const & msg_in) { myNet.queryFromPeboPeer(myNodeId, msg_in); }

        private:
            PeboNet & myNet;
            std::string myNodeId;
        };
    };
}