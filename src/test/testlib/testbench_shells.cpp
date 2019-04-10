#include "testbench_shells.hpp"
#include "connector_peer.hpp"
#include <memory>
#include <vector>

using namespace pebo;
using namespace std;


long TestBenchShells::myCounter = 0;

void TestBenchShells::addShell(shared_ptr<Shell> shell_in)
{
    string id = "testnode" + to_string(++myCounter);
    myShells[id] = shell_in;
}

void TestBenchShells::connect()
{
    // connect pair-wise
    for(auto s1 = myShells.begin(); s1 != myShells.end(); ++s1)
    {
        for(auto s2 = myShells.begin(); s2 != myShells.end(); ++s2)
        {
            if (s1 != s2)
            {
               // connect s1 with s2, using a ConnectedClient
               ConnectorPeer::connect2Nets(s1->second->getPeboNet().get(), s2->second->getPeboNet().get(), s1->first, s2->first);
            }
        }
    }
}

void TestBenchShells::deinit()
{
    // connect pair-wise
    for(auto s = myShells.begin(); s != myShells.end(); ++s)
    {
        s->second->deinit();
    }
}
