#include "store.hpp"
#include <iostream>

using namespace pebo;
using namespace std;

bool Store::findPeer(service_t service_in, endpoint_t endpoint_in, PeerInfo & peerInfo_inout)
{
    // TODO thread-safety !
    // check service
    if (myStore.find(service_in) == myStore.end())
    {
        // service not present
        return false;
    }
    // check endpoint
    if (myStore[service_in].find(endpoint_in) == myStore[service_in].end())
    {
        // endpoint not present
        return false;
    }
    // present
    peerInfo_inout = myStore[service_in][endpoint_in];
    return true;
}

IStore::updateResult_t Store::findAndUpdate(pebo::service_t service_in, pebo::endpoint_t endpoint_in, bool isRemoved_in)
{
    // TODO thread-safe !
    IStore::updateResult_t res = Store::updateResult_t::upd_invalid;
    // prepare new, if needed to add
    timestamp_t now = 0; // TODO
    IStore::PeerInfo newPeer {
        service_in,
        endpoint_in,
        now,
        now,
        isRemoved_in
    };
    // check existing
    PeerInfo oldPeer;
    bool found = findPeer(service_in, endpoint_in, oldPeer);
    if (!found)
    {
        // not found, add it
        addPeer(newPeer);
        return IStore::updateResult_t::upd_addedNew;
    }
    // check if need to update
    if (oldPeer.isRemoved != isRemoved_in)
    {
        // need to overwrite
        addPeer(newPeer);
        return IStore::updateResult_t::upd_updatedValue;
    }
    // update timestamps
    addPeer(newPeer);
    return IStore::updateResult_t::upd_updatedOnlyTime;
}

void Store::addPeer(IStore::PeerInfo peer_in)
{
    // TODO thread-safety
    // check service, add if needed
    if (myStore.find(peer_in.service) == myStore.end())
    {
        // service not present, add it
        myStore[peer_in.service] = ServiceStore_t();
    }
    // add or overwrite
    myStore[peer_in.service][peer_in.endpoint] = peer_in;
    //cerr << "Store::addPeer newcnt: " << countNonremoved() << " " << count() << endl;
}

long Store::count() const
{
    long c = 0;
    // TODO thread safety
    for(auto s = myStore.cbegin(); s != myStore.cend(); ++s)
    {
        c += (*s).second.size();
    }
    return c;
}

long Store::countNonremoved() const
{
    long c = 0;
    // TODO thread safety
    for(auto s = myStore.cbegin(); s != myStore.cend(); ++s)
    {
        for(auto e = (*s).second.cbegin(); e != (*s).second.cend(); ++e)
        {
            if (!(*e).second.isRemoved) ++c;
        }
    }
    return c;
}
