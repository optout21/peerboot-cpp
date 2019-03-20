#pragma once

#include "../include/peerboot.hpp"
#include "ipebo_net.hpp"
#include <string>

namespace pebo
{
    /**
     * Top-level enclosure for the PeerBoot library and client.
     */
    class Shell: public IPeboNetCB
    {
    public:
        Shell();
        ~Shell();
        errorCode init(service_t service_in, endpoint_t endpoint_in, NotificationCB callback_in);
        errorCode deinit();
        // Override methods for testing
        void setPeboNet(IPeboNet* peboNet_in);
        void notifyFromPeboNet(PeerInfo peer_in);

    private:
        void doClientCallback(PeerInfo const & peer_in);
        errorCode doNetBroadcast(PeerInfo const & peer_in);

    private:
        bool myInited;
        PeerInfo myPeer;
        NotificationCB myCallback;
        IPeboNet* myPeboNet;
    };
}
