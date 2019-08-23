#include "pebo_net.hpp"

#include "istore.hpp"
#include "net_client.hpp"

#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;


const string PeboNet::myLocalNodeAddr = "__LOCAL_NODE__";

PeboNet::PeboNet(IStore* store_in) :
myPeboNetCB(nullptr),
myStore(store_in)
//myNodeId("")
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

void PeboNet::listenStarted(int port)
{
    cout << "App: Listening on port " << port << endl;

    // add extra peers for testing (except to self), TODO remove it later
    int n = 3;
    // try to connect to clients
    for (int i = 0; i < n; ++i)
    {
        int port1 = 5000 + i;
        //clis[i] = nullptr;
        // skip connection to self
        if (port != port1)
        {
            addPeer("127.0.0.1", port1);
        }
    }

    /*
    // outgoing clients, predefined, TODO
    auto client = make_shared<NetClientOut>(this, "192.168.0.100", 5000);
    //assert(myPeboNet != nullptr);
    addOutPeer(client->getNodeAddr(), client);
    for (int p = 5000; p <= 5005; ++p)
    {
        if (actualPort != p)
        {
            //cerr << p << " " << actualPort << endl;
            auto client = make_shared<NetClientOut>(this, "localhost", p);
            assert(myPeboNet != nullptr);
            myPeboNet->addPeer(client->getNodeAddr(), client);
        }
    }
    */

    tryOutConnections();
}

void PeboNet::addPeer(std::string const & host_in, int port_in)
{
    int port = port_in;
    // default port
    if (port == 0)
    {
        port = 5000; // TODO default port from const
    }
    auto client = make_shared<NetClientOut>(this, host_in, port);
    //auto cli = dynamic_pointer_cast<IPeboPeer>(client);
    addOutPeer(client->getNodeAddr(), client);
}

void PeboNet::addInPeer(string nodeAddr_in, shared_ptr<IPeboPeer>& peer_in)
{
    assert(peer_in != nullptr);
    lock_guard<recursive_mutex> lock(myPeersInMutex);
    myPeersIn.push_back(PeerInWithId { nodeAddr_in, peer_in });
    /*
    if (myPeers.find(cliaddr) == myPeers.end())
    {
        myPeers[cliaddr] = PeerInfo(cliaddr);
    }
    auto cliIn = dynamic_pointer_cast<NetClientIn>(client_in);
    myPeers[cliaddr].setInClient(cliIn);
    */
}

void PeboNet::addOutPeer(string nodeAddr_in, shared_ptr<NetClientOut>& peer_in)
{
    assert(peer_in != nullptr);
    lock_guard<recursive_mutex> lock(myPeersOutMutex);
    myPeersOut.push_back(PeerOutWithId { nodeAddr_in, peer_in });
}

void PeboNet::inConnectionReceived(string nodeAddr_in, shared_ptr<IPeboPeer>& peer_in)
{
    cout << "App: Connection received; " << nodeAddr_in << endl;
    addInPeer(nodeAddr_in, peer_in);
}

void PeboNet::tryOutConnections()
{
    //cout << "PeboNet::tryOutConnections" << endl;
    // try to connect to clients
    { // lock scope
        lock_guard<recursive_mutex> lock(myPeersOutMutex);
        for (auto i = myPeersOut.begin(); i != myPeersOut.end(); ++i)
        {
            //cout << i->second.myOutFlag << " " << i->second.myStickyOutFlag << " " << i->second.myOutConnTryCount << " " << (i->second.myOutClient == nullptr) << " " << i->second.myOutHost << ":" << i->second.myOutPort << endl;
            //if (i->myOutFlag)
            //{
                //if (i->second.myOutConnTryCount == 0 || i->second.myStickyOutFlag)
                //{
                    // try outgoing connection
                    //cout << "Trying out conn to " << i->second.myOutHost << ":" << i->second.myOutPort << endl;
                    /*if (i->peer == nullptr)
                    {
                        auto peerout = make_shared<NetClientOut>(this, i->second.myOutHost, i->second.myOutPort);
                        i->second.setOutClient(peerout);
                    }*/
                    assert(i->peer != nullptr);
                    //cout << "isConnected " << i->second.myOutClient->isConnected() << endl;
                    //if (!i->peer->isConnected())
                    //{
                        //++i->second.myOutConnTryCount;
                        int res = i->peer->connect();
                        if (res)
                        {
                            // error
                        }
                    //}
                //}
            //}
        }
    }
}

errorCode PeboNet::broadcast(PeerInfo const & peer_in)
{
    return doBroadcastMsg(PeerUpdateMessage(peer_in.service, peer_in.endpoint, peer_in.lastSeen, peer_in.isRemoved, 0), myLocalNodeAddr);
}

errorCode PeboNet::queryRemote(service_t service_in)
{
    return doBroadcastMsg(QueryMessage(service_in, 0), myLocalNodeAddr);
}

/*
void PeboNet::setNodeId(std::string const & nodeId_in)
{ 
    if (myNodeId.length() == 0)
    {
        myNodeId = nodeId_in;
        //cerr << "PeboNet::setNodeId " << myNodeId << endl;
    }
}
*/

void PeboNet::peerUpdateFromPeboPeer(IPeboPeer& peer_in, PeerUpdateMessage const & msg_in)
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
        doBroadcastMsg(msg_in, peer_in.getNodeAddr());
    }
}

void PeboNet::queryFromPeboPeer(IPeboPeer& peer_in, QueryMessage const & msg_in)
{
    // TODO DO not forward query, there is no limit on it
    //doBroadcastMsg(msg_in, nodeId_in);

    //cerr << "PeboNet::queryFromPeboPeer " << myNodeId << " " << service_in << endl;
    doQuery(peer_in, msg_in.getService());
}

void PeboNet::msgFromPeboPeer(IPeboPeer& peer_in, BaseMessage const & msg_in)
{
    //cerr << "PeboNet::msgFromPeboPeer " << nodeId_in << endl;
    FromPeerMessageVisitor visitor(*this, peer_in);
    msg_in.visit(visitor);
}

errorCode PeboNet::doBroadcastMsg(BaseMessage const & msg_in, string origPrevHopNodeAddr_in)
{
    //cerr << "PeboNet::doBroadcastMsg " << msg_in.getType() << " " << origPrevHopNodeId_in << " " << myNetPeers.size() << endl;
    std::vector<PeerOutWithId> netPeersOutCopy;
    { // lock scope
        lock_guard<recursive_mutex> lock(myPeersOutMutex);
        netPeersOutCopy = myPeersOut;
    }
    //cerr << "peers: " << netPeersCopy.size() << endl;
    for(auto i = netPeersOutCopy.begin(); i != netPeersOutCopy.end(); ++i)
    {
        // skip where it came from (previous hop)
        if (i->nodeAddr != origPrevHopNodeAddr_in)
        {
            //cerr << "PeboNet::doBroadcastMsg '" << origPrevHopNodeId_in << "' '" << i->nodeId << "'" << endl;
            i->peer->sendMessage(msg_in);
        }
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doQuery(IPeboPeer& peer_in, service_t service_in)
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
        // send result to net
        //cerr << "QUERY res " << peer_in.getNodeAddr() << " " << i->endpoint << endl;
        peer_in.sendMessage(PeerUpdateMessage(i->service, i->endpoint, i->lastSeen, i->isRemoved, 0));
    }
    return errorCode::err_ok;
}

errorCode PeboNet::doClientCallback(PeerInfo const & peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}