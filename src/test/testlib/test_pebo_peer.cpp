#include "test_pebo_peer.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

TestPeboPeer::TestPeboPeer(pebo::service_t service_in, int num_in) :
myService(service_in),
myEndpoint("endpoint_" + to_string(num_in)),
myNum(num_in),
myId(to_string(num_in)),
myPeboPeerCB(nullptr)
{
}

void TestPeboPeer::setNotifyCB(IPeboPeerCB* peboPeerCB_in)
{
    myPeboPeerCB = peboPeerCB_in;
}

errorCode TestPeboPeer::send(PeerInfo const & peer_in)
{
    cout << "TestPeboPeer " << myEndpoint << ": " << peer_in.endpoint << endl;
}

void TestPeboPeer::start()
{
    myBgThread = move(thread([=]() { this->doBgThread(); return 1; }));
}

void TestPeboPeer::stop()
{
    myBgThreadStop = true;
    myBgThread.join();
}

void TestPeboPeer::doNetCallback(PeerInfo const & peer_in)
{
    assert(myPeboPeerCB != nullptr);
    myPeboPeerCB->notifyFromPeboPeer(myId, peer_in);
}

void TestPeboPeer::doBgThread()
{
    while (!myBgThreadStop)
    {
        for (auto i = 0; i < myNum; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doNetCallback(PeerInfo { myService, myEndpoint, 0, false });
        for (auto i = 0; i < 3 * myNum; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doNetCallback(PeerInfo { myService, myEndpoint, 0, true });
    }
}