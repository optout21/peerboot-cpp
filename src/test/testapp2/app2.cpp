#include "../../include/peerboot.hpp"
#include "../../peerboot/shell.hpp"
#include "../testlib/testbench_shells.hpp"
#include "../testlib/simul_client.hpp"
#include "../../peerboot/timestamp.hpp"
#include "../../peerboot/empty_net_handler.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

using namespace pebo;
using namespace std;

void notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in)
{
    auto lastchar = self_in.endpoint[self_in.endpoint.length() - 1];
    if (lastchar == '0')
    {
        // client 1
        cout << "Notification " << self_in.endpoint << ": " << (peer_in.isRemoved ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << TimeStamp::age(peer_in.lastSeen) << endl;
    }
    else
    {
        //cout << "." << lastchar;
    }
}

int main()
{
    cout << "PeerBoot TestApp2, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;

    pebo::service_t service ("sample.peerboot.io");
    pebo::endpoint_t dummy_endpoint ("dummy_endpoint");
    pebo::endpoint_t endpoint_base ("endpoint_");

    // Create shell components
    auto numClients = 4;
    auto clients = vector<shared_ptr<SimulClient>>();
    auto testBench = TestBenchShells();
    auto emptyNetHandler = shared_ptr<INetHandler>(make_shared<EmptyNetHandler>());
    for(auto i = 0; i < numClients; ++i)
    {
        auto shell = make_shared<Shell>();
        // If EmptyNetHandler is used, there is no networking.  Otherwise peers within the same process communicate over the net.
        shell->setNetHandler(emptyNetHandler);
        pebo::errorCode err = shell->init(::notificationCB, std::vector<Endpoint>(), 0);
        assert(err == errorCode::err_ok);
        testBench.addShell(shell);

        string endpoint = endpoint_base + to_string(i);
        auto client = make_shared<SimulClient>(service, endpoint, shell.get(), (i <= 1), i+2, (i <= 1) ? 6 : 30);
        clients.push_back(client);
    }
    testBench.connect();
    // start clients
    for(auto i = clients.begin(); i != clients.end(); ++i)
    {
        pebo::errorCode err = i->get()->start();
        assert(err == errorCode::err_ok);
    }

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl << "Stopping..." << endl;

    // stop clients
    for(auto i = clients.begin(); i != clients.end(); ++i)
    {
        i->get()->stop();
    }
    testBench.deinit();

    cout << "Done" << endl;
}
