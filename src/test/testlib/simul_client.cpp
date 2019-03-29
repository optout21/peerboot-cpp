#include "simul_client.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

SimulClient::SimulClient(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::Shell* shell_in, int num_in) :
myService(service_in),
myEndpoint(endpoint_in),
myShell(shell_in),
myNum(num_in),
myId(to_string(num_in))
{
}

void SimulClient::notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in)
{
    cout << "Notification: " << (peer_in.isRemoved ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << peer_in.lastSeen << endl;
}

errorCode SimulClient::start()
{
    pebo::errorCode err = myShell->start(myService, myEndpoint);
    if (err)
    {
        cerr << "Could not initialize Shell, err: " << err << " service: " << myService << " endpoint: " << myEndpoint << endl;
        return err;
    }

    myBgThread = move(thread([=]() { this->doBgThread(); return 1; }));
    return errorCode::err_ok;
}

void SimulClient::stop()
{
    myBgThreadStop = true;
    myBgThread.join();
}

void SimulClient::doBroadcast(bool removed_in)
{
    // TODO also do with remove
    assert(myShell != nullptr);
    errorCode err;
    if (!removed_in)
    {
        err = myShell->start(myService, myEndpoint);
    }
    else
    {
        err = myShell->stop();
    }
}

void SimulClient::doBgThread()
{
    //cerr << "SimulClient::doBgThread" << endl;
    while (!myBgThreadStop)
    {
        for (auto i = 0; i < myNum; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doBroadcast(false);
        for (auto i = 0; i < 3 * myNum; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doBroadcast(true);
    }
}

