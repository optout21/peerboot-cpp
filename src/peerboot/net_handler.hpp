#pragma once

#include "inet_handler.hpp"
//#include "pebo_net.hpp"
#include <string>
#include <thread>

namespace pebo
{
    class IPeboNet; // forward

    class NetHandler: public INetHandler
    {
    public:
        NetHandler() = default;
        void init(std::shared_ptr<IPeboNet> peboNet_in);
        errorCode start(int port_in, int tryNextPorts_in);
        errorCode stop();

    private:
        static int doBindPort(int port_in);
        // returns the used port, or <0 for error
        int doListen(int port_in, int tryNextPorts_in);
        int doBgThread();

    private:
        std::shared_ptr<IPeboNet> myPeboNet;
        int myListenSocket;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}
