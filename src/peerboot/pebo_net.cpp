#include "pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

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

errorCode PeboNet::addPeer(shared_ptr<IPeboPeer> const & peer_in)
{
    // TODO threadsafety
    myNetPeers.push_back(peer_in);
}

errorCode PeboNet::broadcast(PeerInfo const & peer_in)
{
    return doBroadcast(peer_in);
}

errorCode PeboNet::doBroadcast(PeerInfo const & peer_in)
{
    // TODO threadsafety
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        i->get()->send(peer_in);
    }
    return errorCode::err_generic;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}