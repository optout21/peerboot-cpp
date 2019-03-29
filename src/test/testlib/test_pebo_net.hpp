#pragma once
#include "../../peerboot/ipebo_net.hpp"
#include <cassert>
#include <memory>
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
        errorCode init(std::string id_in);
        errorCode deinit();
        errorCode addPeer(std::string id_in, std::shared_ptr<IPeboPeer> const & peer_in) { assert(false); };
        errorCode broadcast(PeerInfo const & peer_in);
        errorCode query(service_t service_in) { assert(false); }
        void notifyFromPeboPeer(std::string id_in, PeerInfo peer_in) { assert(false); }
        void queryFromPeboPeer(std::string id_in, service_t service_in) { assert(ffalse); }

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