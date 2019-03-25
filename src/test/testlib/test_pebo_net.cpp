#include "test_pebo_net.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace pebo;
using namespace std;

const int TestPeboNet::myNumPeers;
const std::string TestPeboNet::myServiceName = "test.peerboot.io";

void TestPeboNet::setNotifyCB(std::shared_ptr<IPeboNetCB> peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode TestPeboNet::init()
{
    myBgThread = move(thread([=]() { this->doBgThread(); return 1; }));
    return errorCode::err_ok;
}

errorCode TestPeboNet::deinit()
{
    myBgThread.join();
    return errorCode::err_ok;
}

errorCode TestPeboNet::broadcast(PeerInfo peer_in)
{
    // store in our vector
    myPeers.push_back(peer_in);
    return errorCode::err_ok;
}

errorCode TestPeboNet::doClientCallback(PeerInfo peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}

void TestPeboNet::doBgThread()
{
    myPeerCounter = 0;
    // initial fill
    while (myPeers.size() < myNumPeers)
    {
        addPeer();
    }
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        checkPeers();
    }
}

void TestPeboNet::addPeer()
{
    string endpoint = "enpoint_";
    endpoint.append(to_string(myPeerCounter));
    PeerInfo peer { myServiceName, endpoint, 0 };
    ++myPeerCounter;
    myPeers.push_back(peer);

    doClientCallback(peer);
}

void TestPeboNet::fillPeers()
{
    while (myPeers.size() < myNumPeers)
    {
        addPeer();
    }
}

void TestPeboNet::checkPeers()
{
    // replace a peer
    PeerInfo peer_remove = myPeers[0];
    myPeers.erase(myPeers.begin());
    // notify leaving
    peer_remove.removed = true;
    doClientCallback(peer_remove);
    fillPeers();
}