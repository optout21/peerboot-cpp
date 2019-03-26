#include "../../include/peerboot.hpp"
#include "../../peerboot/shell.hpp"
#include "../testlib/test_pebo_peer.hpp"
#include <iostream>
#include <memory>
#include <vector>

using namespace pebo;
using namespace std;

void notificationCB(pebo::PeerInfo peer_in)
{
    cout << "Notification: " << (peer_in.removed ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << peer_in.last_seen << endl;
}

int main()
{
    cout << "PeerBoot TestApp2, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;

    pebo::service_t service ("sample.peerboot.io");
    pebo::endpoint_t endpoint ("dummy_endpoint");

    // Create components
    auto shell = make_shared<Shell>();
    pebo::errorCode err = shell->init(service, endpoint, ::notificationCB);
    if (err)
    {
        cerr << "Could not initialize PeerBoot library, err: " << err << " service: " << service << " endpoint: " << endpoint << endl;
        return err;
    }
    cout << "PeerBoot library initialized, service: " << service << " endpoint: " << endpoint << endl;

    auto numPeers = 3;
    auto peers = vector<shared_ptr<TestPeboPeer>>();
    for(auto i = 0; i < 3; ++i)
    {
        peers.push_back(make_shared<TestPeboPeer>(service, i+1));
    }
    auto peboNet = shell->getPeboNet();
    for(auto i = peers.begin(); i != peers.end(); ++i)
    {
        i->get()->setNotifyCB(peboNet.get());
        peboNet->addPeer(i->get()->getId(), *i);
    }
    for(auto i = peers.begin(); i != peers.end(); ++i)
    {
        i->get()->start();
    }

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    for(auto i = peers.begin(); i != peers.end(); ++i)
    {
        i->get()->stop();
    }

    err = shell->deinit();
    cout << "PeerBoot library deinitialized, err: " << err << endl;
}
