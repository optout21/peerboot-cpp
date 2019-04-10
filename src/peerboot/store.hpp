#pragma once
#include "istore.hpp"
#include <map>
#include <mutex>
#include <vector>

namespace pebo
{
    /**
     * Store: stores peer info in memory
     */
    class Store: public IStore
    {
    private:
        typedef std::map<endpoint_t, IStore::PeerInfo> ServiceStore_t;
        typedef std::map<service_t, ServiceStore_t> Store_t;

    public:
        Store() = default;
        updateResult_t findAndUpdate(service_t service_in, endpoint_t endpoint_in, bool isRemoved_in);
        long count() const;
        std::vector<IStore::PeerInfo> query(service_t service_in) const;
        void clear();

    private:
        bool findPeer(service_t service_in, endpoint_t endpoint_in, PeerInfo & peerInfo_inout);
        void addPeer(IStore::PeerInfo peer_in);
        long countPrivate() const;
        long countNonremoved() const;

    private:
        Store_t myStore;
        mutable std::mutex myMutex; // not recursive_mutex
    };
}
