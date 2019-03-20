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
        errorCode init(service_t service_in, endpoint_t endpoint_in, notification_cb callback_in);
        errorCode deinit();
        // Override methods for testing
        void setPeboNet(IPeboNet* peboNet_in);
        void notifyFromPeboNet(peer_t peer_in);

    private:
        void doClientCallback(peer_t const & peer_in);
        errorCode doNetBroadcast(peer_t const & peer_in);

    private:
        bool myInited;
        peer_t myPeer;
        notification_cb myCallback;
        IPeboNet* myPeboNet;
    };
}
