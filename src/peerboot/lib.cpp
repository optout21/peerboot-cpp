/**
 * Implementation of public global methods.
 */

#include "../include/peerboot.hpp"
#include "shell.hpp"
#include <cassert>

// Global single shell object
pebo::shell* global_instance = nullptr;

pebo::errorCode pebo::init(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::notification_cb callback_in)
{
    // TODO: thread-safe access to global instance
    if (::global_instance != nullptr)
    {
        // already initialized
        return pebo::errorCode::err_libAlreadyInited;
    }
    assert (::global_instance == nullptr);
    
    // create and init library
    pebo::shell* shell_new = new pebo::shell();
    pebo::errorCode err = shell_new->init(callback_in);
    if (err)
    {
        delete shell_new;
        return err;
    }
    // save it
    ::global_instance = shell_new;

    return pebo::errorCode::err_ok;
}

pebo::errorCode pebo::deinit()
{
    if (::global_instance == nullptr)
    {
        // no lib, nothing to do
        return pebo::errorCode::err_libNotInited;
    }
    assert(::global_instance != nullptr);

    pebo::shell* shell_old = ::global_instance;
    ::global_instance = nullptr;

    // deinit lib
    pebo::errorCode err = shell_old->deinit();
    delete shell_old;

    return err;
}
