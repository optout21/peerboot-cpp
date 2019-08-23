#include "peers.hpp"

using namespace std;

void pebo::Peers::add(pebo::PeerInfo const & peer_in)
{
    myPeers[peer_in.endpoint] = peer_in;
}
