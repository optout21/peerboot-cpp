#pragma once
#include "../../peerboot/ipebo_peer.hpp"
#include "../../peerboot/ipebo_net.hpp"
#include <memory>

namespace pebo
{
    /**
     * Connects to another PeBo Peer.
     */
    class ConnectorPeer: public IPeboPeer
    {
    protected:
        ConnectorPeer(std::string nodeAddr_in);

    public:
        static std::pair<std::shared_ptr<ConnectorPeer>, std::shared_ptr<ConnectorPeer>> createConnectedPair(std::string id1_in, std::string id2_in);
        static void connect2Nets(IPeboNet* net1_in, IPeboNet* net2_in, std::string nodeAddr1_in, std::string nodeAddr2_in);
        void setPeer(ConnectorPeer* peer_in);
        void setNotifyCB(IPeboPeerCB* peboPeerCB_in);
        // Send a message to this peer
        int sendMessage(BaseMessage const & msg_in);
        std::string getNodeAddr() const { return myNodeAddr; }
    
    private:
        void messageFromNet(BaseMessage const & msg_in);

    private:
        IPeboPeerCB* myPeboPeerCB;
        std::string myNodeAddr;
        ConnectorPeer* myPeer;
    };
}