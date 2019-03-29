#include "connector_peer.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

ConnectorPeer::ConnectorPeer(std::string id_in) :
myId(id_in)
{
}

pair<shared_ptr<ConnectorPeer>, shared_ptr<ConnectorPeer>> ConnectorPeer::createConnectedPair(std::string id1_in, std::string id2_in)
{
    // create a pair of connected peers
    auto peer1 = shared_ptr<ConnectorPeer>(new ConnectorPeer(id1_in));
    auto peer2 = shared_ptr<ConnectorPeer>(new ConnectorPeer(id2_in));
    // link them together
    peer1->setPeer(peer2.get());
    peer2->setPeer(peer1.get());
    return pair<shared_ptr<ConnectorPeer>, shared_ptr<ConnectorPeer>>(peer1, peer2);
}

void ConnectorPeer::connect2Nets(IPeboNet* net1_in, IPeboNet* net2_in)
{
    assert(net1_in != net2_in);
    auto id1 = net1_in->getNodeId();
    auto id2 = net2_in->getNodeId();
    auto peerPair = createConnectedPair(id1, id2);
    net1_in->addPeer(id2, peerPair.first);
    peerPair.first->setNotifyCB(net1_in);
    net2_in->addPeer(id1, peerPair.second);
    peerPair.second->setNotifyCB(net2_in);
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
