#include "shell.hpp"
#include "pebo_net.hpp"
#include "net_handler.hpp"
#include "store.hpp"
#include "timestamp.hpp"

#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

Shell::Shell() :
myInited(false),
myCallback(nullptr),
myPeboNet(nullptr),
myNetHandler(nullptr),
myStore(unique_ptr<IStore>(make_unique<Store>()))
{
}

Shell::~Shell()
{
    deinit();
}

errorCode Shell::init(NotificationCB callback_in, vector<Endpoint> extraPeers_in, int listenPort_in)
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
    // components: NetHandler -- create if not set
    if (myNetHandler == nullptr)
    {
        // default component
        myNetHandler = make_shared<NetHandler>();
    }
    assert(myNetHandler != nullptr);
    // components: PeboNet -- create if not yet set
    if (myPeboNet == nullptr)
    {
        // default component
        assert(myStore != nullptr);
        myPeboNet = make_shared<PeboNet>(myStore.get());
        myPeboNet->setNotifyCB(this);
    }
    assert(myPeboNet != nullptr);

    // add bootstrap peers
    myPeboNet->addPeer("node1.peerboot.io", 5000);  // TODO default port from const
    myPeboNet->addPeer("node2.peerboot.io", 5000);  // TODO default port from const
    if (extraPeers_in.size() > 0)
    {
        for (auto i(extraPeers_in.begin()), n(extraPeers_in.end()); i != n; ++i)
        {
            myPeboNet->addPeer(i->getHost(), i->getPort());
        }
    }

    myNetHandler->init(myPeboNet);

    // Listening port
    int listenPort = listenPort_in;
    int listenPortRange = 1;
    if (listenPort_in == 0)
    {
        // default
        listenPort = 5000;
        listenPortRange = 25;
    }
    assert(myNetHandler != nullptr);
    int actualPort = myNetHandler->start(listenPort, listenPortRange);
    //cerr << "actual port " << actualPort << endl;
    if (actualPort < 0)
    {
        return errorCode::err_networkListen;
    }

    return errorCode::err_ok;
}

errorCode Shell::start(service_t service_in, endpoint_t endpoint_in)
{
    // save client info
    myPeer = PeerInfo { service_in, endpoint_in, TimeStamp::now() };

    assert(myPeboNet != nullptr);
    errorCode netres = myPeboNet->start();
    if (netres)
    {
        return netres;
    }

    // store this client's endpoint and broadcast it to the net
    assert(myStore != nullptr);
    myStore->findAndUpdate(service_in, endpoint_in, false, myPeer.lastSeen);
    broadcast_refresh();

    // query for other known endpoints of this service
    doNetQuery(service_in);

    return errorCode::err_ok;
}

errorCode Shell::stop()
{
    // broadcast good bye to the net
    broadcast_bye();

    assert(myPeboNet != nullptr);
    myPeboNet->stop();
    assert(myNetHandler != nullptr);
    myNetHandler->stop();

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
    if (myNetHandler != nullptr)
    {
        myNetHandler->stop();
        myNetHandler = nullptr;
    }
    if (myPeboNet != nullptr)
    {
        //errorCode res2 = myPeboNet->deinit();
        //if (res2 && !res) res = res2;
        myPeboNet = nullptr;
    }
    myCallback = nullptr;

    return res;
}

void Shell::setNetHandler(std::shared_ptr<INetHandler> & netHandler_in)
{
    myNetHandler = netHandler_in;
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
    errorCode res = myPeboNet->queryRemote(service_in);
    return res;
}
