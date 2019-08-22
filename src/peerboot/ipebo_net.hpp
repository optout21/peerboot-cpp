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
        //virtual errorCode init() = 0;
        //virtual errorCode deinit() = 0;
        virtual errorCode start() = 0;
        virtual errorCode stop() = 0;
        /// Called when server is listening on a port already
        virtual void listenStarted(int port) = 0;
        /// Add an outpoing pebo peer connection, host:port
        virtual void addPeer(std::string const & host_in, int port_in) = 0;
        /// Called when a new incoming connection is received
        virtual void inConnectionReceived(std::string nodeAddr_in, std::shared_ptr<IPeboPeer>& peer_in) = 0;
        //virtual errorCode addPeer(std::string nodeAddr_in, std::shared_ptr<IPeboPeer> const & peer_in) = 0;
        // Broadcast peer info towards the network
        virtual errorCode broadcast(PeerInfo const & peer_in) = 0;
        // Perform query from the peers, results will come asynchronously
        virtual errorCode queryRemote(service_t service_in) = 0;
        //virtual std::string getNodeId() = 0;
        //virtual void setNodeId(std::string const & nodeId_in) = 0;
    };
}