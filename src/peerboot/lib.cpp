/**
 * Implementation of public global methods.
 */

#include "../include/peerboot.hpp"
#include "shell.hpp"
#include <cassert>
#include <mutex>
#include <vector>

using namespace pebo;

// Global single shell object
Shell* global_instance = nullptr;
std::mutex global_init_mutex;

errorCode pebo::init(service_t service_in, endpoint_t endpoint_in, NotificationCB callback_in)
{
    return initEx(service_in, endpoint_in, callback_in, 0, NULL, 0);
}

errorCode pebo::initEx(service_t service_in, endpoint_t endpoint_in, NotificationCB callback_in, int pbPeerCnt_in, endpoint_t pbPeerList_in[], int pbListenPort_in)
{
    std::lock_guard<std::mutex> lock(global_init_mutex);
    if (::global_instance != nullptr)
    {
        // already initialized
        return errorCode::err_libAlreadyInited;
    }
    assert (::global_instance == nullptr);

    // process extra peers, if any
    std::vector<Endpoint> extraPeers;
    if (pbPeerCnt_in > 0)
    {
        for (int i = 0; i < pbPeerCnt_in; ++i) extraPeers.push_back(Endpoint(pbPeerList_in[i]));
    }

    // create and init library
    Shell* shell_new = new Shell();
    errorCode err = shell_new->init(callback_in, extraPeers, pbListenPort_in);
    if (err)
    {
        delete shell_new;
        return err;
    }
    err = shell_new->start(service_in, endpoint_in);
    if (err)
    {
        delete shell_new;
        return err;
    }
    // save it
    ::global_instance = shell_new;

    return errorCode::err_ok;
}

errorCode pebo::deinit()
{
    std::lock_guard<std::mutex> lock(global_init_mutex);
    if (::global_instance == nullptr)
    {
        // no lib, nothing to do
        return errorCode::err_libNotInited;
    }
    assert(::global_instance != nullptr);

    Shell* shell_old = ::global_instance;
    ::global_instance = nullptr;

    // deinit lib
    errorCode err = shell_old->deinit();
    delete shell_old;

    return err;
}
