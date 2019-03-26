#include "connector_peer.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

ConnectorPeer::ConnectorPeer(std::string id_in) :
myId(id_in)
{
}

pair<shared_ptr<ConnectorPeer>, shared_ptr<ConnectorPeer>> ConnectorPeer::createConnectedPair(string id_in)
{
    // create a pair of connected peers
    auto peer1 = shared_ptr<ConnectorPeer>(new ConnectorPeer(id_in));
    auto peer2 = shared_ptr<ConnectorPeer>(new ConnectorPeer(id_in));
    // link them together
    peer1->setPeer(peer2.get());
    peer2->setPeer(peer1.get());
    return pair<shared_ptr<ConnectorPeer>, shared_ptr<ConnectorPeer>>(peer1, peer2);
}

void ConnectorPeer::setPeer(ConnectorPeer* peer_in)
{
    assert(myPeer == nullptr);
    myPeer = peer_in;
    // TODO myId = peer_in->
}

void ConnectorPeer::setNotifyCB(IPeboPeerCB* peboPeerCB_in)
{
    myPeboPeerCB = peboPeerCB_in;
}

errorCode ConnectorPeer::send(PeerInfo const & peer_in)
{
    //cerr << "ConnectorPeer::send " << myId << endl;
    // pass it to our peer
    assert(myPeer != nullptr);
    myPeer->doNetCallback(peer_in);
}

void ConnectorPeer::doNetCallback(PeerInfo const & peer_in)
{
    assert(myPeboPeerCB != nullptr);
    myPeboPeerCB->notifyFromPeboPeer(myId, peer_in);
}
