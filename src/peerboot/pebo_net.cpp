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
    // TODO threadsafety
    myNetPeers.push_back(PeerWithId { nodeId_in, peer_in });
    //cerr << "PeboNet::addPeer " << id_in << " " << myNetPeers.size() << endl;
}

errorCode PeboNet::broadcast(PeerInfo const & peer_in)
{
    return doBroadcast(peer_in, myNodeId);
}

errorCode PeboNet::query(service_t service_in)
{
    return doBroadcastQuery(service_in, myNodeId);
}

void PeboNet::notifyFromPeboPeer(string nodeId_in, PeerInfo peer_in)
{
    //cerr << "PeboNet::notifyFromPeboPeer " << myId << " from " << id_in << " " << peer_in.endpoint << endl;
    IStore::updateResult_t updateRes = myStore->findAndUpdate(peer_in.service, peer_in.endpoint, peer_in.isRemoved);
    if (updateRes == IStore::updateResult_t::upd_updatedOnlyTime ||
        updateRes == IStore::updateResult_t::upd_noChangeNeeded)
    {
        // it was already known, do nothing
    }
    else
    {
        // peer info added or updated, forward it
        doClientCallback(peer_in);
        doBroadcast(peer_in, nodeId_in);
    }
}

void PeboNet::queryFromPeboPeer(std::string nodeId_in, service_t service_in)
{
    // TODO DO not forward query, there is no limit on it
    //doBroadcastQuery(service_in, nodeId_in);

    //cerr << "PeboNet::queryFromPeboPeer " << myId << " " << service_in << endl;
    doQuery(nodeId_in, service_in);
}

errorCode PeboNet::doBroadcast(PeerInfo const & peer_in, string originatorNode)
{
    //cerr << "PeboNet::doBroadcast " << myNetPeers.size() << endl;
    // TODO threadsafety
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        // skip originator
        //cerr << "PeboNet::doBroadcast " << originatorNode << " " << i->nodeId << endl;
        if (i->nodeId != originatorNode)
        {
            i->peer->send(peer_in);
        }
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doBroadcastQuery(service_t service_in, string originatorNode)
{
    //cerr << "PeboNet::doBroadcastQuery " << myNetPeers.size() << endl;
    // TODO threadsafety
    for(auto i = myNetPeers.begin(); i != myNetPeers.end(); ++i)
    {
        // skip originator
        //cerr << "PeboNet::doBroadcastQuery " << originatorNode << " " << i->nodeId << endl;
        if (i->nodeId != originatorNode)
        {
            i->peer->query(service_in);
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
    //cerr << "QUERY " << id_in << " " << result.size() << endl;
    for(auto i = result.cbegin(); i != result.cend(); ++i)
    {
        // broadcast result to net.  Originator is myself
        //cerr << "QUERY res " << id_in << " " << i->endpoint << endl;
        doBroadcast(pebo::PeerInfo { i->service, i->endpoint, i->lastSeenTime, i->isRemoved}, myNodeId);
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}