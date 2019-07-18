#include "connector_peer.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

ConnectorPeer::ConnectorPeer(std::string nodeAddr_in) :
myNodeAddr(nodeAddr_in)
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

void ConnectorPeer::connect2Nets(IPeboNet* net1_in, IPeboNet* net2_in, std::string nodeAddr1_in, std::string nodeAddr2_in)
{
    assert(net1_in != net2_in);
    //cerr << "ConnectorPeer::connect2Nets " << id1 << " " << id2 << endl;
    auto peerPair = createConnectedPair(nodeAddr1_in, nodeAddr2_in);
    // cross connect
    auto peer1 = dynamic_pointer_cast<IPeboPeer>(peerPair.first);
    auto peer2 = dynamic_pointer_cast<IPeboPeer>(peerPair.second);
    net1_in->inConnectionReceived(nodeAddr2_in, peer1);
    peerPair.first->setNotifyCB(net1_in);
    net2_in->inConnectionReceived(nodeAddr1_in, peer2);
    peerPair.second->setNotifyCB(net2_in);
}

void ConnectorPeer::setPeer(ConnectorPeer* peer_in)
{
    assert(myPeer == nullptr);
    myPeer = peer_in;
}

void ConnectorPeer::setNotifyCB(IPeboPeerCB* peboPeerCB_in)
{
    myPeboPeerCB = peboPeerCB_in;
}

int ConnectorPeer::sendMessage(BaseMessage const & msg_in)
{
    // pass it to our peer
    assert(myPeer != nullptr);
    myPeer->messageFromNet(msg_in);
    return 0;
}

void ConnectorPeer::messageFromNet(BaseMessage const & msg_in)
{
    assert(myPeboPeerCB != nullptr);
    myPeboPeerCB->msgFromPeboPeer(*this, msg_in);
}
