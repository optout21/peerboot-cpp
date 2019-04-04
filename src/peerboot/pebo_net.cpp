#include "pebo_net.hpp"
#include "istore.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

PeboNet::PeboNet(IStore* store_in) :
myPeboNetCB(nullptr),
myStore(store_in)
{
    assert(myStore != nullptr);
}

void PeboNet::setNotifyCB(IPeboNetCB* peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode PeboNet::init(std::string nodeId_in)
{
    myNodeId = nodeId_in;
    return errorCode::err_ok;
}

errorCode PeboNet::deinit()
{
    return errorCode::err_ok;
}

errorCode PeboNet::addPeer(string nodeId_in, shared_ptr<IPeboPeer> const & peer_in)
{
    lock_guard<recursive_mutex> lock(myNetPeersMutex);
    myNetPeers.push_back(PeerWithId { nodeId_in, peer_in });
    //cerr << "PeboNet::addPeer " << nodeId_in << " " << myNetPeers.size() << endl;
}

errorCode PeboNet::broadcast(PeerInfo const & peer_in)
{
    return doBroadcastPeer(PeerUpdateMessage(peer_in.service, peer_in.endpoint, peer_in.lastSeen, peer_in.isRemoved, 0), myNodeId);
}

errorCode PeboNet::queryRemote(service_t service_in)
{
    return doBroadcastQuery(QueryMessage(service_in, 0), myNodeId);
}

void PeboNet::peerUpdateFromPeboPeer(string nodeId_in, PeerUpdateMessage const & msg_in)
{
    //cerr << "PeboNet::peerUpdateFromPeboPeer " << myNodeId << " from " << nodeId_in << " " << peer_in.endpoint << endl;
    IStore::updateResult_t updateRes = myStore->findAndUpdate(msg_in.getService(), msg_in.getEndpoint(), msg_in.getIsRemoved());
    if (updateRes == IStore::updateResult_t::upd_updatedOnlyTime ||
        updateRes == IStore::updateResult_t::upd_noChangeNeeded)
    {
        // it was already known, do nothing
    }
    else
    {
        PeerInfo peer { msg_in.getService(), msg_in.getEndpoint(), msg_in.getLastSeen(), msg_in.getIsRemoved() };
        // peer info added or updated, forward it
        doClientCallback(peer);
        doBroadcastPeer(msg_in, nodeId_in);
    }
}

void PeboNet::queryFromPeboPeer(std::string nodeId_in, QueryMessage const & msg_in)
{
    // TODO DO not forward query, there is no limit on it
    //doBroadcastQuery(msg_in, nodeId_in);

    //cerr << "PeboNet::queryFromPeboPeer " << myNodeId << " " << service_in << endl;
    doQuery(nodeId_in, msg_in.getService());
}

void PeboNet::msgFromPeboPeer(std::string nodeId_in, BaseMessage const & msg_in)
{
   MessageFromPeerVisitor visitor(*this, nodeId_in);
   msg_in.visit(visitor);
}

errorCode PeboNet::doBroadcastPeer(PeerUpdateMessage const & msg_in, string originatorNodeId)
{
    //cerr << "PeboNet::doBroadcastPeer " << myNetPeers.size() << endl;
    lock_guard<recursive_mutex> lock(myNetPeersMutex);
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        // skip originator
        //cerr << "PeboNet::doBroadcastPeer " << originatorNodeId << " " << i->nodeId << endl;
        if (i->nodeId != originatorNodeId)
        {
            i->peer->sendMsg(msg_in);
        }
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doBroadcastQuery(QueryMessage const & msg_in, string originatorNodeId)
{
    //cerr << "PeboNet::doBroadcastQuery " << myNetPeers.size() << endl;
    lock_guard<recursive_mutex> lock(myNetPeersMutex);
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        // skip originator
        //cerr << "PeboNet::doBroadcastQuery " << originatorNodeId << " " << i->nodeId << endl;
        if (i->nodeId != originatorNodeId)
        {
            i->peer->sendMsg(msg_in);
        }
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doQuery(std::string nodeId_in, service_t service_in)
{
    // query from store and broadcast results
    auto result = myStore->query(service_in);
    if (result.size() == 0)
    {
        // no results
        return errorCode::err_ok;
    }
    //cerr << "QUERY " << nodeId_in << " " << result.size() << endl;
    for(auto i = result.cbegin(); i != result.cend(); ++i)
    {
        // broadcast result to net.  Originator is myself
        //cerr << "QUERY res " << nodeId_in << " " << i->endpoint << endl;
        doBroadcastPeer(PeerUpdateMessage(i->service, i->endpoint, i->lastSeen, i->isRemoved, 0), myNodeId);
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}