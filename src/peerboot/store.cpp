#include "store.hpp"
#include "timestamp.hpp"

#include <iostream>

using namespace pebo;
using namespace std;

bool Store::findPeer(service_t service_in, endpoint_t endpoint_in, PeerInfo & peerInfo_inout)
{
    // thread-safety: private, from outside
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

IStore::updateResult_t Store::findAndUpdate(pebo::service_t service_in, pebo::endpoint_t endpoint_in, bool isRemoved_in, pebo::timestamp_t lastSeen_in)
{
    lock_guard<mutex> lock(myMutex);
    IStore::updateResult_t res = Store::updateResult_t::upd_invalid;
    // prepare new, if needed to add
    timestamp_t now = TimeStamp::now();
    IStore::PeerInfo newPeer {
        service_in,
        endpoint_in,
        now,
        lastSeen_in,
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
    // update timestamps, preserve addedTime
    newPeer.addedTime = oldPeer.addedTime;
    addPeer(newPeer);
    return IStore::updateResult_t::upd_updatedOnlyTime;
}

void Store::addPeer(IStore::PeerInfo peer_in)
{
    // thread-safety: private, from outside
    // check service, add if needed
    if (myStore.find(peer_in.service) == myStore.end())
    {
        // service not present, add it
        myStore[peer_in.service] = ServiceStore_t();
    }
    // add or overwrite
    //long oldCount = countPrivate();
    myStore[peer_in.service][peer_in.endpoint] = peer_in;
    /*
    long newCount = countPrivate();
    if (newCount != oldCount)
    {
        cerr << "Store::addPeer newcnt: " << countNonremoved() << " " << countPrivate() << endl;
    }
    */
}

long Store::count() const
{
    lock_guard<mutex> lock(myMutex);
    return countPrivate();
}

long Store::countPrivate() const
{
    long c = 0;
    for(auto s = myStore.cbegin(); s != myStore.cend(); ++s)
    {
        c += (*s).second.size();
    }
    return c;
}

vector<IStore::PeerInfo> Store::query(service_t service_in) const
{
    lock_guard<mutex> lock(myMutex);
    vector<IStore::PeerInfo> result;
    auto endpoints = myStore.find(service_in);
    if (endpoints == myStore.end())
    {
        // service not present
        return result;
    }
    for(auto e = endpoints->second.cbegin(); e != endpoints->second.cend(); ++e)
    {
        if (!(*e).second.isRemoved) result.push_back((*e).second);
    }
    return result;
}

void Store::clear()
{
    lock_guard<mutex> lock(myMutex);
    myStore.clear();
}

long Store::countNonremoved() const
{
    // thread-safety: private, from outside
    long c = 0;
    for(auto s = myStore.cbegin(); s != myStore.cend(); ++s)
    {
        for(auto e = s->second.cbegin(); e != s->second.cend(); ++e)
        {
            if (!e->second.isRemoved) ++c;
        }
    }
    return c;
}
