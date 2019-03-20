#include "test_pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;

void TestPeboNet::setNotifyCB(IPeboNetCB* peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode TestPeboNet::init()
{
    return errorCode::err_ok;
}

errorCode TestPeboNet::broadcast(peer_t peer_in)
{
    // Do not send to any network, but send back to client
    errorCode res = doClientCallback(peer_in);
    return res;
}

errorCode TestPeboNet::doClientCallback(peer_t peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}