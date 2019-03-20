#pragma once
#include "../../peerboot/ipebo_net.hpp"
#include <thread>
#include <vector>

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
        errorCode init();
        errorCode deinit();
        errorCode broadcast(PeerInfo peer_in);

    private:
        errorCode doClientCallback(PeerInfo peer_in);
        void doBgThread();
        void addPeer();
        void fillPeers();
        void checkPeers();

    private:
        IPeboNetCB* myPeboNetCB;
        std::thread myBgThread;
        static const int myNumPeers = 5;
        static const std::string myServiceName;
        std::vector<PeerInfo> myPeers;
        int myPeerCounter;
    };
}