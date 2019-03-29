#include "testbench_shells.hpp"
#include "connector_peer.hpp"
#include <memory>
#include <vector>

using namespace pebo;
using namespace std;

void TestBenchShells::addShell(shared_ptr<Shell> shell_in)
{
    myShells.push_back(shell_in);
}

void TestBenchShells::connect()
{
    // connect pair-wise
    for(auto s1 = 0; s1 < myShells.size(); ++s1)
    {
        for(auto s2 = s1 + 1; s2 < myShells.size(); ++s2)
        {
            if (s1 != s2)
            {
               // connect s1 with s2, using a ConnectedClient
               ConnectorPeer::connect2Nets(myShells[s1]->getPeboNet().get(), myShells[s2]->getPeboNet().get());
            }
        }
    }
}

void TestBenchShells::deinit()
{
    // connect pair-wise
    for(auto i = 0; i < myShells.size(); ++i)
    {
        myShells[i]->deinit();
    }
}
