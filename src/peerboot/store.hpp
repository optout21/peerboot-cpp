#pragma once
#include "istore.hpp"
#include <map>

namespace pebo
{
    /**
     * Store: stores peer info in memory
     */
    class Store: public IStore
    {
    private:
        typedef std::map<pebo::endpoint_t, pebo::IStore::PeerInfo> ServiceStore_t;
        typedef std::map<pebo::service_t, ServiceStore_t> Store_t;

    public:
        Store() = default;
        updateResult_t findAndUpdate(pebo::service_t service_in, pebo::endpoint_t endpoint_in, bool isRemoved_in);
        bool findPeer(pebo::service_t service_in, pebo::endpoint_t endpoint_in, PeerInfo & peerInfo_inout);
        long count() const;

    private:
        void addPeer(pebo::IStore::PeerInfo peer_in);
        long countNonremoved() const;

    private:
        Store_t myStore;
    };
}
