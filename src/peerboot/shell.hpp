#pragma once

#include "../include/peerboot.hpp"
#include "ipebo_net.hpp"
#include "istore.hpp"
#include <memory>
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
        void setPeboNet(std::shared_ptr<IPeboNet> & peboNet_in);
        std::shared_ptr<IPeboNet> getPeboNet() const { return myPeboNet; }
        void notifyFromPeboNet(PeerInfo peer_in);
        errorCode broadcast_refresh();
        errorCode broadcast_bye();

    private:
        void doClientCallback(PeerInfo const & peer_in);
        errorCode doNetBroadcast(PeerInfo const & peer_in);

    private:
        bool myInited;
        PeerInfo myPeer;
        NotificationCB myCallback;
        std::shared_ptr<IPeboNet> myPeboNet;
        std::unique_ptr<IStore> myStore;
    };
}
