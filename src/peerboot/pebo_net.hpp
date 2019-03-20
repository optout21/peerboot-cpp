#pragma once
#include "ipebo_net.hpp"

namespace pebo
{
    /**
     * Represents the PeerBoot network.
     * TODO: fill it
     */
    class PeboNet : public IPeboNet
    {
    public:
        PeboNet() = default;
        void setNotifyCB(IPeboNetCB* peboNetCB_in);
        errorCode init();
        errorCode deinit();
        errorCode broadcast(PeerInfo peer_in);

    private:
        errorCode doClientCallback(PeerInfo peer_in);

    private:
        IPeboNetCB* myPeboNetCB;
    };
}