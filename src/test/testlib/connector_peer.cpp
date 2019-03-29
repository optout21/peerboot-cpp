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
    //cerr << "ConnectorPeer::send " << myId << " " << peer_in.endpoint << endl;
    // pass it to our peer
    assert(myPeer != nullptr);
    myPeer->doNetSendCallback(peer_in);
    return errorCode::err_ok;
}

errorCode ConnectorPeer::query(service_t service_in)
{
    // pass it to our peer
    assert(myPeer != nullptr);
    myPeer->doNetQueryCallback(service_in);
    return errorCode::err_ok;
}

void ConnectorPeer::doNetSendCallback(PeerInfo const & peer_in)
{
    //cerr << "ConnectorPeer::doNetSendCallback " << myId << " " << peer_in.endpoint << endl;
    assert(myPeboPeerCB != nullptr);
    myPeboPeerCB->notifyFromPeboPeer(myId, peer_in);
}

void ConnectorPeer::doNetQueryCallback(service_t service_in)
{
    assert(myPeboPeerCB != nullptr);
    myPeboPeerCB->queryFromPeboPeer(myId, service_in);
}
