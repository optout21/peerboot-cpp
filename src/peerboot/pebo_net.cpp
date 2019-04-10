#include "pebo_net.hpp"

#include "istore.hpp"
//#include "net_client.hpp"

#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

PeboNet::PeboNet(IStore* store_in) :
myPeboNetCB(nullptr),
myStore(store_in),
myNodeId("?")
{
    assert(myStore != nullptr);
}

void PeboNet::setNotifyCB(IPeboNetCB* peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

/*
errorCode PeboNet::init()
{
    return errorCode::err_ok;
}

errorCode PeboNet::deinit()
{
    return errorCode::err_ok;
}
*/

errorCode PeboNet::start()
{
    return errorCode::err_ok;
}

errorCode PeboNet::stop()
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
    return doBroadcastMsg(PeerUpdateMessage(peer_in.service, peer_in.endpoint, peer_in.lastSeen, peer_in.isRemoved, 0), myNodeId);
}

errorCode PeboNet::queryRemote(service_t service_in)
{
    return doBroadcastMsg(QueryMessage(service_in, 0), myNodeId);
}

void PeboNet::setNodeId(std::string const & nodeId_in)
{ 
    if (myNodeId.length() == 0 || myNodeId == "?")
    {
        myNodeId = nodeId_in;
        //cerr << "PeboNet::setNodeId " << myNodeId << endl;
    }
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
        //cerr << "New peer info, from " << nodeId_in << " endpoint " << msg_in.getEndpoint() << endl;
        doClientCallback(peer);
        doBroadcastMsg(msg_in, nodeId_in);
    }
}

void PeboNet::queryFromPeboPeer(std::string nodeId_in, QueryMessage const & msg_in)
{
    // TODO DO not forward query, there is no limit on it
    //doBroadcastMsg(msg_in, nodeId_in);

    //cerr << "PeboNet::queryFromPeboPeer " << myNodeId << " " << service_in << endl;
    doQuery(nodeId_in, msg_in.getService());
}

void PeboNet::msgFromPeboPeer(std::string nodeId_in, BaseMessage const & msg_in)
{
    //cerr << "PeboNet::msgFromPeboPeer " << nodeId_in << endl;
    FromPeerMessageVisitor visitor(*this, nodeId_in);
    msg_in.visit(visitor);
}

errorCode PeboNet::doBroadcastMsg(BaseMessage const & msg_in, string originatorNodeId)
{
    //cerr << "PeboNet::doBroadcastMsg " << msg_in.getType() << " " << originatorNodeId << " " << myNetPeers.size() << endl;
    std::vector<PeerWithId> netPeersCopy;
    { // lock scope
        lock_guard<recursive_mutex> lock(myNetPeersMutex);
        netPeersCopy = myNetPeers;
    }
    //cerr << "peers: " << netPeersCopy.size() << endl;
    for(auto i = netPeersCopy.begin(); i != netPeersCopy.end(); ++i)
    {
        // skip originator
        if (i->nodeId != originatorNodeId)
        {
            //cerr << "PeboNet::doBroadcastMsg '" << originatorNodeId << "' '" << i->nodeId << "'" << endl;
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
        doBroadcastMsg(PeerUpdateMessage(i->service, i->endpoint, i->lastSeen, i->isRemoved, 0), myNodeId);
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}