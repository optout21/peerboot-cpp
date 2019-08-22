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
        err_networkListen,
        err_networkGeneric,
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
     * @service_in: The service identifier of the P2P network.
     * @endpoint_in: The endpoint of this client (service endpoint, not peerboot endpoint).
     * @callback_in: Callback to be called when notifying about new peers.
     * @return: 0 for success, >0 for errors
     */
    pebo::errorCode init(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::NotificationCB callback_in);

    /*
     * Initialize the PeerBoot library and PeerBoot peer client, extended version.
     * One instance per process.
     * @service_in: The service identifier of the P2P network.
     * @endpoint_in: The endpoint of this client (service endpoint, not peerboot endpoint).
     * @callback_in: Callback to be called when notifying about new peers.
     * @pbPeerCnt_in: Number of extra peerboot peers provided, may be 0.
     * @pbPeerList_in: Extra peerboot peers provided (its size is pbPeerCnt_in).
     * @pbListenPort_in: Listening port to be used by PeerBoot, 0 means default.
     * @return: 0 for success, >0 for errors
     */
    pebo::errorCode initEx(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::NotificationCB callback_in, int pbPeerCnt_in, pebo::endpoint_t pbPeerList_in[], int pbListenPort_in);

    /**
     * De-initialize the library and PeerBoot peer client.
     */ 
    pebo::errorCode deinit();
}

