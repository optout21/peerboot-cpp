#pragma once
#include "ipebo_net.hpp"
#include <memory>

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
        void setNotifyCB(std::shared_ptr<IPeboNetCB> peboNetCB_in);
        errorCode init();
        errorCode deinit();
        errorCode broadcast(PeerInfo peer_in);

    private:
        errorCode doClientCallback(PeerInfo peer_in);

    private:
        std::shared_ptr<IPeboNetCB> myPeboNetCB;
    };
}