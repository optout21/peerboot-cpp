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
    class NetClientOut; // forward

    /**
     * Represents the PeerBoot network.
     * TODO: fill it
     */
    class PeboNet : public IPeboNet
    {
    public:
        struct PeerInWithId
        {
        public:
            std::string nodeAddr;
            std::shared_ptr<IPeboPeer> peer;
        };
        struct PeerOutWithId
        {
        public:
            std::string nodeAddr;
            std::shared_ptr<NetClientOut> peer;
        };
        PeboNet(IStore* store_in);
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        //errorCode init();
        //errorCode deinit();
        errorCode start();
        errorCode stop();
        /// Called when server is listening on a port already
        void listenStarted(int port);
        /// Add an outpoing pebo peer connection to host:port
        void addPeer(std::string const & host_in, int port_in);
        /// Called when a new incoming connection is received
        void inConnectionReceived(std::string nodeAddr_in, std::shared_ptr<IPeboPeer>& peer_in);
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode queryRemote(service_t service_in);
        //std::string getNodeId() { return myNodeId; }
        //void setNodeId(std::string const & nodeId_in);
        // A message received from the peer
        void msgFromPeboPeer(IPeboPeer& peer_in, BaseMessage const & msg_in);
        static const std::string myLocalNodeAddr;

    private:
        void addInPeer(std::string nodeAddr_in, std::shared_ptr<IPeboPeer>& peer_in);
        void addOutPeer(std::string nodeAddr_in, std::shared_ptr<NetClientOut>& peer_in);
        void tryOutConnections();
        void peerUpdateFromPeboPeer(IPeboPeer& peer_in, PeerUpdateMessage const & msg_in);
        void queryFromPeboPeer(IPeboPeer& peer_in, QueryMessage const & msg_in);
        // origPrevHopNodeId_in: the nodeId of the node where this came from, to avoid sending back; or myLocalNodeId for locally-originated messages.
        errorCode doBroadcastMsg(BaseMessage const & msg_in, std::string origPrevHopNodeAddr_in);
        errorCode doQuery(IPeboPeer& peer_in, service_t service_in);
        errorCode doClientCallback(PeerInfo const & peer_in);

    private:
        //std::string myNodeId;
        IPeboNetCB* myPeboNetCB;
        std::vector<PeerInWithId> myPeersIn;
        std::vector<PeerOutWithId> myPeersOut;
        std::recursive_mutex myPeersInMutex;
        std::recursive_mutex myPeersOutMutex;
        IStore* myStore;

        // Put as inner class to access private methods
        class FromPeerMessageVisitor: public MessageVisitorBase
        {
        public:
            FromPeerMessageVisitor(PeboNet & net_in, IPeboPeer& peer_in) :
                MessageVisitorBase(), myNet(net_in), myPeer(peer_in) { }
            virtual ~FromPeerMessageVisitor() = default;
            void peerUpdate(PeerUpdateMessage const & msg_in) { myNet.peerUpdateFromPeboPeer(myPeer, msg_in); }
            void query(QueryMessage const & msg_in) { myNet.queryFromPeboPeer(myPeer, msg_in); }

        private:
            PeboNet& myNet;
            IPeboPeer& myPeer;
        };
    };
}