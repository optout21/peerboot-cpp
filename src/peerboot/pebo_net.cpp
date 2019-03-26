#include "pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

PeboNet::PeboNet() :
myPeboNetCB(nullptr)
{
}

void PeboNet::setNotifyCB(IPeboNetCB* peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode PeboNet::init()
{
    return errorCode::err_ok;
}

errorCode PeboNet::deinit()
{
    return errorCode::err_ok;
}

errorCode PeboNet::addPeer(long id_in, shared_ptr<IPeboPeer> const & peer_in)
{
    // TODO threadsafety
    myNetPeers.push_back(PeerWithId { id_in, peer_in });
}

errorCode PeboNet::broadcast(PeerInfo const & peer_in)
{
    return doBroadcast(peer_in, 0);
}

void PeboNet::notifyFromPeboPeer(long id_in, PeerInfo peer_in)
{
    doClientCallback(peer_in);
    doBroadcast(peer_in, id_in);
}

errorCode PeboNet::doBroadcast(PeerInfo const & peer_in, long originatorPeer)
{
    // TODO threadsafety
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        // skip originator
        if (i->id != originatorPeer)
        {
            i->peer->send(peer_in);
        }
    }
    return errorCode::err_generic;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}