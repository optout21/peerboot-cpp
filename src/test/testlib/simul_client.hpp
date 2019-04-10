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
        SimulClient(service_t service_in, endpoint_t endpoint_in, Shell* shell_in, bool verbose_in, int waitTime_in, int keepTime_in);
        // also starts underlying shell
        errorCode start();
        void stop();

    private:
        static void notificationCB(PeerInfo self_in, PeerInfo peer_in);
        errorCode doStart();
        errorCode doStop();
        void doBgThread();

    private:
        service_t myService;
        endpoint_t myEndpoint;
        Shell* myShell;
        bool myVerbose;
        int myStartWait;
        int myStartKeep;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}