#include "empty_net_handler.hpp"
#include "pebo_net.hpp"

#include <cassert>
#include <iostream>

using namespace pebo;
using namespace std;

long EmptyNetHandler::myIdCounter = 0;

void EmptyNetHandler::init(std::shared_ptr<IPeboNet> peboNet_in)
{
    myPeboNet = peboNet_in;
    assert(myPeboNet != nullptr);
}

int EmptyNetHandler::start(int port_in, int tryNextPorts_in)
{
    return 0;
}

int EmptyNetHandler::stop()
{
    return 0;
}
