#include "simul_client.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

SimulClient::SimulClient(pebo::service_t service_in, pebo::endpoint_t endpoint_in, pebo::Shell* shell_in, bool verbose_in, int waitTime_in, int keepTime_in) :
myService(service_in),
myEndpoint(endpoint_in),
myShell(shell_in),
myVerbose(verbose_in),
myStartWait(waitTime_in),
myStartKeep(keepTime_in),
myNodeId(shell_in->getPeboNet()->getNodeId())
{
    if (myVerbose) cout << "SimulClient " << myNodeId << "; " << myStartWait << " " << myStartKeep << endl;
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

errorCode SimulClient::doStart()
{
    if (myVerbose) cout << "SimulClient " << myNodeId << ": doStart" << endl;
    assert(myShell != nullptr);
    errorCode err = myShell->start(myService, myEndpoint);
    return err;
}

errorCode SimulClient::doStop()
{
    if (myVerbose) cout << "SimulClient " << myNodeId << ": doStop" << endl;
    assert(myShell != nullptr);
    errorCode err = myShell->stop();
    return err;
}

void SimulClient::doBgThread()
{
    //cerr << "SimulClient::doBgThread" << endl;
    while (!myBgThreadStop)
    {
        for (auto i = 0; i < myStartWait; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doStart();
        for (auto i = 0; i < myStartKeep; ++i)
        {
            if (myBgThreadStop) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (myBgThreadStop) break;
        doStop();
    }
}

