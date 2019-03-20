#include "shell.hpp"
#include <cassert>

pebo::shell::shell() :
myInited(false),
myCallback(nullptr)
{
}

pebo::shell::~shell()
{
    deinit();
}

pebo::errorCode pebo::shell::init(pebo::notification_cb callback_in)
{
    // TODO thread-safe access to inited
    if (myInited)
    {
        return pebo::errorCode::err_libAlreadyInited;
    }
    assert(!myInited);
    assert(myCallback == nullptr);
    myInited = true;

    myCallback = callback_in;

    return pebo::errorCode::err_ok;
}

pebo::errorCode pebo::shell::deinit()
{
    if (!myInited)
    {
        return pebo::errorCode::err_libNotInited;
    }
    assert(myInited);
    myInited = false;

    myCallback = nullptr;

    return pebo::errorCode::err_ok;
}

void pebo::shell::doCallback(pebo::peer_t peer_in)
{
    // TODO thread safety
    if (myCallback == nullptr)
    {
        return;
    }
    assert(myCallback != nullptr);
    myCallback(peer_in);
}