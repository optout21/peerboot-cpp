#pragma once

#include "../include/peerboot.hpp"
#include "ipebo_net.hpp"
#include "inet_handler.hpp"
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
        errorCode init(NotificationCB callback_in);
        errorCode start(service_t service_in, endpoint_t endpoint_in);
        errorCode stop();
        errorCode deinit();
        // Override methods for testing
        void setNetHandler(std::shared_ptr<INetHandler> & netHandler_in);
        void setPeboNet(std::shared_ptr<IPeboNet> & peboNet_in);
        std::shared_ptr<IPeboNet> getPeboNet() const { return myPeboNet; }
        void notifyFromPeboNet(PeerInfo peer_in);
        errorCode broadcast_refresh();

    private:
        errorCode broadcast_bye();
        void doClientCallback(PeerInfo const & peer_in);
        errorCode doNetBroadcast(PeerInfo const & peer_in);
        errorCode doNetQuery(service_t service_in);

    private:
        bool myInited;
        PeerInfo myPeer;
        NotificationCB myCallback;
        std::shared_ptr<INetHandler> myNetHandler;
        std::shared_ptr<IPeboNet> myPeboNet;
        std::unique_ptr<IStore> myStore;
    };
}
