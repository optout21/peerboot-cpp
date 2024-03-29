#pragma once
#include "../../peerboot/ipebo_net.hpp"
#include <cassert>
#include <memory>
#include <thread>
#include <vector>

#include <iostream>

namespace pebo
{
    /**
     * Simulated PeerBoot network.
     */
    class TestPeboNet : public IPeboNet
    {
    public:
        TestPeboNet() = default;
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        //errorCode init();
        //errorCode deinit();
        errorCode start();
        errorCode stop();
        void listenStarted(int port) { }
        void addPeer(std::string const & host_in, int port_in) { }
        void inConnectionReceived(std::string nodeAddr_in, std::shared_ptr<IPeboPeer>& peer_in) { }
        //errorCode addPeer(std::string nodeId_in, std::shared_ptr<IPeboPeer> const & peer_in) { assert(false); };
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode queryRemote(service_t service_in);
        //std::string getNodeId() { assert(false); return "???"; }
        //void setNodeId(std::string const & nodeId_in) { assert(false); }
        void msgFromPeboPeer(IPeboPeer& peer_in, BaseMessage const & msg_in) { assert(false); }

    private:
        errorCode doClientCallback(PeerInfo const & peer_in);
        void doBgThread();
        void addPeer();
        void fillPeers();
        void checkPeers();

    private:
        IPeboNetCB* myPeboNetCB;
        std::thread myBgThread;
        bool myBgThreadStop;
        static const int myNumPeers = 5;
        static const std::string myServiceName;
        std::vector<PeerInfo> myPeers;
        int myPeerCounter;
    };
}