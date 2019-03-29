#include "shell.hpp"
#include "pebo_net.hpp"
#include "store.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

int Shell::myIdCounter = 0;

Shell::Shell() :
myInited(false),
myCallback(nullptr),
myPeboNet(nullptr),
myStore(unique_ptr<IStore>(make_unique<Store>()))
{
}

Shell::~Shell()
{
    deinit();
}

errorCode Shell::init(NotificationCB callback_in)
{
    // TODO thread-safe access to inited
    if (myInited)
    {
        return errorCode::err_libAlreadyInited;
    }
    assert(!myInited);
    assert(myCallback == nullptr);
    myInited = true;

    // callback
    myCallback = callback_in;

    // components: Store -- created upfront
    assert (myStore != nullptr);
    // components: PeboNet -- create if not yet set
    if (myPeboNet == nullptr)
    {
        // default component
        assert (myStore != nullptr);
        myPeboNet = make_shared<PeboNet>(myStore.get());
        myPeboNet->setNotifyCB(this);
    }
    assert(myPeboNet != nullptr);
    ++myIdCounter;
    errorCode res = myPeboNet->init("id" + to_string(myIdCounter));
    if (res)
    {
        myInited = false;
        return res;
    }

    return errorCode::err_ok;
}

errorCode Shell::start(service_t service_in, endpoint_t endpoint_in)
{
    // save client info
    myPeer = PeerInfo { service_in, endpoint_in, 0 };  // TODO time

    // broadcast this client to the net
    broadcast_refresh();

    // query for other known endpoints of this service
    doNetQuery(service_in);

    return errorCode::err_ok;
}

errorCode Shell::stop()
{
    // broadcast good bye to the net
    broadcast_bye();

    // clear store
    myStore->clear();

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

errorCode Shell::doNetQuery(service_t service_in)
{
    // TODO thread safety
    assert(myPeboNet != nullptr);
    errorCode res = myPeboNet->query(service_in);
    return res;
}
