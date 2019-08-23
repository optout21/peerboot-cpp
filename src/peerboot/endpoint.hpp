#pragma once

#include "../include/peerboot.hpp"

#include <string>

namespace pebo
{
    /**
     * Endpoint handling.
     */
    class Endpoint
    {
    public:
        /// Construct from string
        Endpoint(std::string endpointStr_in);
        std::string getHost() const { return myHost; }
        int getPort() const { return myPort; }
    private:
        std::string myHost;
        int myPort;
    };
}
