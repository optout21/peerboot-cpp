#include "pebo_net.hpp"
#include <cassert>
#include <iostream>

using namespace pebo;

void PeboNet::setNotifyCB(std::shared_ptr<IPeboNetCB> peboNetCB_in)
{
    myPeboNetCB = peboNetCB_in;
}

errorCode PeboNet::init()
{
    return errorCode::err_ok;
}

errorCode PeboNet::deinit()
{
    return errorCode::err_ok;
}

errorCode PeboNet::broadcast(PeerInfo peer_in)
{
    // TODO send to network
    return errorCode::err_generic;
}

errorCode PeboNet::doClientCallback(PeerInfo peer_in)
{
    assert(myPeboNetCB != nullptr);
    myPeboNetCB->notifyFromPeboNet(peer_in);
}