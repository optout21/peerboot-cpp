#pragma once
#include "../../peerboot/ipebo_net.hpp"

namespace pebo
{
    class TestPeboNet : public IPeboNet
    {
    public:
        TestPeboNet() = default;
        errorCode broadcast(peer_t peer_in);
    };
}