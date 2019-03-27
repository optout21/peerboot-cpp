#include "../../include/peerboot.hpp"
#include "../../peerboot/shell.hpp"
#include "../testlib/connector_peer.hpp"
#include "../testlib/simul_client.hpp"
#include <iostream>
#include <memory>
#include <vector>

using namespace pebo;
using namespace std;

void notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in)
{
    cout << "Notification " << self_in.endpoint << ": " << (peer_in.isRemoved ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << peer_in.lastSeen << endl;
}

int main()
{
    cout << "PeerBoot TestApp2, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;

    pebo::service_t service ("sample.peerboot.io");
    pebo::endpoint_t dummy_endpoint ("dummy_endpoint");
    pebo::endpoint_t endpoint_base ("endpoint_");

    // Create shell components
    auto numClients = 4;
    auto shells = vector<shared_ptr<Shell>>();
    auto clients = vector<shared_ptr<SimulClient>>();
    for(auto i = 0; i < numClients; ++i)
    {
        string endpoint = endpoint_base + to_string(i);
        auto shell = make_shared<Shell>();
        auto client = make_shared<SimulClient>(service, endpoint, shell.get(), i+1);
        pebo::errorCode err = shell->init(service, endpoint, ::notificationCB);
        if (err)
        {
            cerr << "Could not initialize PeerBoot library, err: " << err << " service: " << service << " endpoint: " << endpoint << endl;
            return err;
        }
        shells.push_back(shell);
        clients.push_back(client);
    }
    // connect pair-wise
    for(auto s1 = 0; s1 < shells.size(); ++s1)
    {
        for(auto s2 = s1 + 1; s2 < shells.size(); ++s2)
        {
            if (s1 != s2)
            {
               // connect s1 with s2, using a ConnectedClient
               string id = "cp_" + to_string(s1) + "_" + to_string(s2);
               auto peerPair = ConnectorPeer::createConnectedPair(id);
               shells[s1]->getPeboNet()->addPeer(id, peerPair.first);
               peerPair.first->setNotifyCB(shells[s1]->getPeboNet().get());
               shells[s2]->getPeboNet()->addPeer(id, peerPair.second);
               peerPair.second->setNotifyCB(shells[s2]->getPeboNet().get());
            }
        }
    }

    // start clients
    for(auto i = clients.begin(); i != clients.end(); ++i)
    {
        i->get()->start();
    }

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    // stop clients
    for(auto i = clients.begin(); i != clients.end(); ++i)
    {
        i->get()->stop();
    }
    // Deinit shells
    for(auto i = shells.begin(); i != shells.end(); ++i)
    {
        i->get()->deinit();
    }

    cout << "Done" << endl;
}
