/**
 * A sample Client.
 */
#pragma once
#include "../../include/peerboot.hpp"
#include "../../peerboot/shell.hpp"
#include <thread>

namespace pebo
{
    class SimulClient
    {
    public:
        SimulClient(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::Shell* shell_in, int num_in);
        // also starts underlying shell
        pebo::errorCode start();
        void stop();

    private:
        static void notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in);
        void doBroadcast(bool removed_in);
        void doBgThread();

    private:
        pebo::service_t myService;
        pebo::endpoint_t myEndpoint;
        pebo::Shell* myShell;
        int myNum;
        std::string myId;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}