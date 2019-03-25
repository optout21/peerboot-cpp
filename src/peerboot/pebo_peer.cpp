#include "pebo_peer.hpp"
#include <iostream>

using namespace pebo;
using namespace std;

errorCode PeboPeer::send(PeerInfo const & peer_in)
{
    cerr << "PeboPeer send " << peer_in.service << " " << peer_in.endpoint << "\n";
}