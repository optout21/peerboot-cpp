/**
 * Implementation of public global methods.
 */

#include "../include/peerboot.hpp"
#include "shell.hpp"
#include <cassert>
#include <mutex>

using namespace pebo;

// Global single shell object
Shell* global_instance = nullptr;
std::mutex global_init_mutex;

errorCode pebo::init(service_t service_in, endpoint_t endpoint_in, NotificationCB callback_in)
{
    std::lock_guard<std::mutex> lock(global_init_mutex);
    if (::global_instance != nullptr)
    {
        // already initialized
        return errorCode::err_libAlreadyInited;
    }
    assert (::global_instance == nullptr);
    
    // create and init library
    Shell* shell_new = new Shell();
    errorCode err = shell_new->init(callback_in);
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
