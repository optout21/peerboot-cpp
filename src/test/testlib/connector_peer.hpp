#pragma once
#include "../../peerboot/ipebo_peer.hpp"
#include <memory>

namespace pebo
{
    /**
     * Connects to another PeBo Peer.
     */
    class ConnectorPeer: public IPeboPeer
    {
    protected:
        ConnectorPeer(std::string id_in);

    public:
        static std::pair<std::shared_ptr<ConnectorPeer>, std::shared_ptr<ConnectorPeer>> createConnectedPair(std::string id_in);
        void setPeer(ConnectorPeer* peer_in);
        void setNotifyCB(IPeboPeerCB* peboPeerCB_in);
        // Send a peer info towards this peer
        errorCode send(PeerInfo const & peer_in);
        // Send a query request to this peer, results will come later asynchronously as peer updates
        errorCode query(service_t service_in);
    
    private:
        void doNetSendCallback(PeerInfo const & peer_in);
        void doNetQueryCallback(service_t service_in);

    private:
        IPeboPeerCB* myPeboPeerCB;
        std::string myId;
        ConnectorPeer* myPeer;
    };
}