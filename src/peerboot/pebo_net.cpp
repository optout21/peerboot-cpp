#include "pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;

void PeboNet::setNotifyCB(IPeboNetCB* peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode PeboNet::init()
{
    return errorCode::err_ok;
}

errorCode PeboNet::broadcast(peer_t peer_in)
{
    // TODO send to network
    return errorCode::err_generic;
}

errorCode PeboNet::doClientCallback(peer_t peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}