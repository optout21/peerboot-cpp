/**
 * Top-level enclosure for the PeerBoot library and client.
 */

#pragma once

#include "../include/peerboot.hpp"
#include <string>

namespace pebo
{
    class shell
    {
    public:
        shell();
        ~shell();
        pebo::errorCode init(pebo::notification_cb callback_in);
        pebo::errorCode deinit();

    private:
        void doCallback(pebo::peer_t peer_in);

    private:
        bool myInited;
        pebo::notification_cb myCallback;
    };
}
