#pragma once
#include "../include/peerboot.hpp"
#include <vector>

namespace pebo
{
    /**
     * Interface for Store, storing peer info
     */
    class IStore
    {
    public:
        /**
         * Describes a stored peer
         */
        struct PeerInfo
        {
        public:
            pebo::service_t service;
            pebo::endpoint_t endpoint;
            pebo::timestamp_t addedTime;
            pebo::timestamp_t lastSeenTime;
            // TODO originating node
            bool isRemoved;
        };

        enum updateResult_t : int16_t
        {
            upd_invalid = 0,
            upd_addedNew,
            upd_updatedValue,
            upd_updatedOnlyTime,
            upd_noChangeNeeded
        };

    public:
        virtual updateResult_t findAndUpdate(pebo::service_t service_in, pebo::endpoint_t endpoint_in, bool isRemoved_in) = 0;
        virtual bool findPeer(pebo::service_t service_in, pebo::endpoint_t endpoint_in, PeerInfo & peerInfo_inout) = 0;
        virtual std::vector<IStore::PeerInfo> query(service_t service_in) const = 0;
        virtual void clear() = 0;
    };
}
