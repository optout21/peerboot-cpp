#include "shell.hpp"
#include "pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

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

errorCode Shell::init(service_t service_in, endpoint_t endpoint_in, NotificationCB callback_in)
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
    myPeer = PeerInfo { service_in, endpoint_in, 0 };  // TODO time
    myCallback = callback_in;

    if (myPeboNet == nullptr)
    {
        // default component
        myPeboNet = make_shared<PeboNet>();
        myPeboNet->setNotifyCB(this);
    }
    assert(myPeboNet != nullptr);
    errorCode res = myPeboNet->init();
    if (res)
    {
        return res;
    }

    // broadcast this client to the net
    broadcast_refresh();

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

    errorCode res = errorCode::err_ok;
    if (myPeboNet != nullptr)
    {
        errorCode res2 = myPeboNet->deinit();
        if (res2 && !res) res = res2;
        myPeboNet = nullptr;
    }
    myCallback = nullptr;

    return res;
}

void Shell::setPeboNet(shared_ptr<IPeboNet> & peboNet_in)
{
    myPeboNet = peboNet_in;
}

void Shell::notifyFromPeboNet(PeerInfo peer_in)
{
    // callback from network, notify client
    doClientCallback(peer_in);
}

errorCode Shell::broadcast_refresh()
{
    return doNetBroadcast(PeerInfo { myPeer.service, myPeer.endpoint, myPeer.lastSeen, false });
}

errorCode Shell::broadcast_bye()
{
    return doNetBroadcast(PeerInfo { myPeer.service, myPeer.endpoint, myPeer.lastSeen, true });
}

void Shell::doClientCallback(PeerInfo const & peer_in)
{
    // TODO thread safety
    if (myCallback == nullptr)
    {
        return;
    }
    assert(myCallback != nullptr);
    myCallback(myPeer, peer_in);
}

errorCode Shell::doNetBroadcast(PeerInfo const & peer_in)
{
    //cerr << "Shell::doNetBroadcast" << endl;
    // TODO thread safety
    assert(myPeboNet != nullptr);
    errorCode res = myPeboNet->broadcast(peer_in);
    return res;
}

