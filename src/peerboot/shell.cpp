#include "shell.hpp"
#include <cassert>

using namespace pebo;

Shell::Shell() :
myInited(false),
myCallback(nullptr),
myPeboNet(nullptr)
{
}

Shell::~Shell()
{
    deinit();
}

errorCode Shell::init(service_t service_in, endpoint_t endpoint_in, notification_cb callback_in)
{
    // TODO thread-safe access to inited
    if (myInited)
    {
        return errorCode::err_libAlreadyInited;
    }
    assert(!myInited);
    assert(myCallback == nullptr);
    myInited = true;

    // save client info
    myPeer = peer_t { service_in, endpoint_in, 0};  // TODO time
    myCallback = callback_in;
    // TODO create myPeboNet

    // broadcast this client to the net
    doNetBroadcast(myPeer);

    return errorCode::err_ok;
}

errorCode Shell::deinit()
{
    if (!myInited)
    {
        return errorCode::err_libNotInited;
    }
    assert(myInited);
    myInited = false;

    myCallback = nullptr;

    return errorCode::err_ok;
}

void Shell::setPeboNet(IPeboNet* peboNet_in)
{
    if (myPeboNet != nullptr)
    {
        delete myPeboNet;
        myPeboNet = nullptr;
    }
    myPeboNet = peboNet_in;
}

void Shell::doClientCallback(peer_t const & peer_in)
{
    // TODO thread safety
    if (myCallback == nullptr)
    {
        return;
    }
    assert(myCallback != nullptr);
    myCallback(peer_in);
}

errorCode Shell::doNetBroadcast(peer_t const & peer_in)
{
    // TODO thread safety
    assert(myPeboNet != nullptr);
    errorCode res = myPeboNet->broadcast(peer_in);
    return res;
}

