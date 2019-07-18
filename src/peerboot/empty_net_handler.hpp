#pragma once

#include "../include/peerboot.hpp"
#include "inet_handler.hpp"
//#include "pebo_net.hpp"

namespace pebo
{
    class IPeboNet; // forward

    /**
     * Empty placeholder NetHandler.
     * TODO: move to test once not needed in prod.
     */
    class EmptyNetHandler: public INetHandler
    {
    public:
        EmptyNetHandler() = default;
        void init(std::shared_ptr<IPeboNet> peboNet_in);
        int start(int port_in, int tryNextPorts_in);
        int stop();
        
    private:
        std::shared_ptr<IPeboNet> myPeboNet;
        static long myIdCounter;
    };
}
