/**
 * PeerBoot library
 */

#pragma once

#include <string>

namespace pebo
{
    // Represents a service describtion, string up to 256 chars.  
    // Examples: 'peerboot.io'
    typedef std::string service_t;

    // Represents a networking endpoint.  Format is service-specific, typically IP endpoint (IPv6 recommended).
    // Represented as ASCII string. Maximum length: 256 chars.
    typedef std::string endpoint_t;

    enum errorCode : int
    {
        err_ok = 0,
        err_generic = 1,
        err_libNotInited,
        err_libAlreadyInited,
        err_libConfigErrorPeboNet,
    };

    /**
     * Timestamp, number of seconds since Jan 1 2019 UTC
     */
    typedef uint32_t timestamp_t;

    /**
     * Describes a peer (client peer, not PB peer).
     */
    struct PeerInfo
    {
    public:
        pebo::service_t service;
        pebo::endpoint_t endpoint;
        pebo::timestamp_t lastSeen;
        bool isRemoved;
    };

    /**
     * Callback for notification of new endpoints.
     * Called by the library.  Called from another thread.
     */
    typedef void (*NotificationCB)(pebo::PeerInfo self_in, pebo::PeerInfo peer_in);

    /*
     * Initialize the PeerBoot library and PeerBoot peer client.
     * One instance per process.
     * @return: 0 for success, >0 for errors
     */
    pebo::errorCode init(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::NotificationCB callback_in);

    /**
     * De-initialize the library and PeerBoot peer client.
     */ 
    pebo::errorCode deinit();
}

