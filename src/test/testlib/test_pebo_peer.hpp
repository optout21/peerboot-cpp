#pragma once
#include "../../peerboot/ipebo_peer.hpp"
#include <thread>

namespace pebo
{
    /**
     * Represents a PeerBoot peer.
     */
    class TestPeboPeer: public IPeboPeer
    {
    public:
        TestPeboPeer(pebo::service_t service_in, long id_in);
        long getId() const { return myId; }
        void setNotifyCB(IPeboPeerCB* peboPeerCB_in);
        // Send a peer info towards this peer
        errorCode send(PeerInfo const & peer_in);
        void start();
        void stop();

    private:
        void doNetCallback(PeerInfo const & peer_in);
        void doBgThread();

    private:
        pebo::service_t myService;
        pebo::endpoint_t myEndpoint;
        long myId;
        IPeboPeerCB* myPeboPeerCB;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}