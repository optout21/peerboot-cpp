/*
 * PeerBoot demo app.
 *
 * Sample usage:
 *  On host1:
 *   peerboot_demoapp -ep HOST1 -port 6001
 *  On host2:
 *   peerboot_demoapp -ep HOST2 -peer host1:6001
 */

#include "../../include/peerboot.hpp"
#include "peers.hpp"
#include "../../peerboot/timestamp.hpp"

#include <iostream>
#include <ctime>

using namespace std;

struct AppParams
{
    string endPoint;
    int pbListenPort;
    string pbExtraPeer;
    void print()
    {
        cout << "Service endpoint:        " << endPoint << endl;
        cout << "PeerBoot extra peer:     " << pbExtraPeer << endl;
        cout << "PeerBoot listening port: " << pbListenPort << endl;
        cout << endl;
    }
};

pebo::Peers gPeers;

void notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in)
{
    cout << "Notification: " << (peer_in.isRemoved ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << peer_in.lastSeen << endl;
    if (!peer_in.isRemoved)
    {
        gPeers.add(peer_in);
    }
    // Print out peers
    auto peers = gPeers.getAll();
    cout << peers.size() << " peers:" << endl;
    auto now = pebo::TimeStamp::now();
    for (auto i(peers.begin()), n(peers.end()); i != n; ++i)
    {
        auto age = now - i->second.lastSeen;
        cout << "  ep " << i->first << " age " << age << endl;
    }
}

void usage(AppParams const & params_in)
{
    cout << "PeerBoot Demo Client, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;
    cout << endl;
    cout << "Usage:  peerboot_demoapp [options]" << endl;
    cout << "  -ep [endpoint]     Service endpoint, only a data to PeerBoot.  Default: " << params_in.endPoint << endl;
    cout << "  -peer [endpoint]   Extra PeerBoot peer.  Optional.  Example: -peer localhost:5500" << endl;
    cout << "  -port [port]       PeerBoot listening port.  0 for default.  Default: " << params_in.pbListenPort << endl;
    cout << "Simple example:" << endl;
    cout << "  peerboot_demoapp -ep EP1_5000" << endl;
    cout << "  peerboot_demoapp -ep EP2_5005 -port 5005 -peer localhost:5000" << endl;
    cout << endl;
}

void processArgs(AppParams & params_inout, int argn, char ** argc)
{
    for (int i = 0; i < argn; ++i)
    {
        if (string(argc[i]) == "-ep")
        {
            if (i + 1 >= argn) break;
            ++i;
            params_inout.endPoint = argc[i];
        }
        else if (string(argc[i]) == "-peer")
        {
            if (i + 1 >= argn) break;
            ++i;
            params_inout.pbExtraPeer = argc[i];
        }
        else if (string(argc[i]) == "-port")
        {
            if (i + 1 >= argn) break;
            ++i;
            params_inout.pbListenPort = std::stoi(argc[i]);
        }
    }
}

int main(int argn, char ** argc)
{
    AppParams appParams { "demo_endpoint", 0, "" };

    usage(appParams);
    processArgs(appParams, argn, argc);
    appParams.print();

    pebo::service_t service ("demo.peerboot.io");
    pebo::endpoint_t endpoint (appParams.endPoint);

    pebo::endpoint_t peers[1];
    peers[0] = pebo::endpoint_t(appParams.pbExtraPeer);
    pebo::errorCode err = pebo::initEx(service, endpoint, ::notificationCB, 1, peers, appParams.pbListenPort);
    if (err)
    {
        cerr << "Could not initialize PeerBoot library, err: " << err << " service: " << service << " endpoint: " << endpoint << endl;
        return err;
    }
    cout << "PeerBoot library initialized, service: " << service << " endpoint: " << endpoint << endl;

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    err = pebo::deinit();
    cout << "PeerBoot library deinitialized, err: " << err << endl;
}
