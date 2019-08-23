#pragma once

#include "../../include/peerboot.hpp"

#include <string>
#include <map>

namespace pebo
{
    /// Collection of peers
    class Peers
    {
    public:
        Peers() { }
        void add(pebo::PeerInfo const & peer_in);
        std::map<pebo::endpoint_t, pebo::PeerInfo> getAll() const { return std::map<pebo::endpoint_t, pebo::PeerInfo>(myPeers); }
    private:
        std::map<pebo::endpoint_t, pebo::PeerInfo> myPeers;
    };
}
