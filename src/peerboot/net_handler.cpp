#include "net_handler.hpp"

#include "ipebo_net.hpp"
#include "net_client.hpp"

#include <uv.h>

#include <cassert>
#include <iostream>
#include <thread>

using namespace pebo;
using namespace std;


uv_loop_t* NetHandler::myUvLoop = nullptr;

uv_loop_t* NetHandler::getUvLoop()
{
    if (myUvLoop == nullptr)
    {
        myUvLoop = new uv_loop_t();
        ::uv_loop_init(myUvLoop);
    }
    assert(myUvLoop != nullptr);
    return myUvLoop;
}

void NetHandler::deleteUvLoop()
{
    uv_loop_t* local = myUvLoop;
    myUvLoop = nullptr;
    if (local != nullptr)
    {
        delete local;
    }
}

int NetHandler::runLoop()
{
    //cout << "NetHandler::runLoop " << endl;

    uv_loop_t* loop = getUvLoop();
    //cerr << "UV LOOP starting" << endl;
    int res = ::uv_run(loop, UV_RUN_DEFAULT);
    if (res)
    {
        cerr << "Nonzero from uv_run(): " << res << " " << ::uv_err_name(res) << endl;
    }
    //cerr << "UV LOOP stopped" << endl;
    res = ::uv_loop_close(loop);
    if (res)
    {
        if (res == -EBUSY)
        {
            cerr << "closing pending handles..." << endl;
            // close handles
            ::uv_walk(loop, NetHandler::on_walk, NULL);
        }
        else
        {
            cerr << "Nonzero from uv_loop_close(): " << res << " " << ::uv_err_name(res) << endl;
        }
    }
    //cerr << "uv loop closed" << endl;
    deleteUvLoop();

    return 0;
}

int NetHandler::startUvLoop()
{
    myBgThreadStop = false;

    uv_loop_t* loop = getUvLoop();
    // async handle to be able to awake loop when needed
    myUvAsync = new uv_async_t();
    int res = ::uv_async_init(loop, myUvAsync, NULL);
    assert(res == 0);

    // start loop in backround thread
    myBgThread = move(thread([=]() { return this->doBgThread(); }));

    return 0;
}

int NetHandler::stopUvLoop()
{
    myBgThreadStop = true;
    // wake loop
    int res = ::uv_async_send(myUvAsync);
    assert(res == 0);
    //cerr << "Stopping UV loop..." << endl;
    ::uv_stop(getUvLoop());
    //cerr << "UV uv_stop() returned" << endl;
    // thread should end, wait for it
    myBgThread.join();
    //cerr << "Bg thread joined" << endl;
    return 0;
}

void NetHandler::init(std::shared_ptr<IPeboNet> peboNet_in)
{
    myPeboNet = peboNet_in;
}

int NetHandler::start(int port_in, int tryNextPorts_in)
{
    int actualPort = doListen(port_in, tryNextPorts_in);
    if (actualPort <= 0)
    {
        return actualPort;
    }
    int res = startUvLoop();
    if (res < 0) return res;
    if (res > 0) return -res;
    return actualPort;
}

/*
    // outgoing clients, predefined, TODO
    for (int p = 5001; p <= 5005; ++p)
    {
        if (actualPort != p)
        {
            //cerr << p << " " << actualPort << endl;
            auto client = make_shared<NetClientOut>(myPeboNet.get(), "localhost", p);
            assert(myPeboNet != nullptr);
            myPeboNet->addPeer(client->getNodeId(), client);
        }
    }
    
    return errorCode::err_ok;
}
*/

int NetHandler::stop()
{
    return stopUvLoop();
}

void NetHandler::on_close(uv_handle_t* handle)
{
    //cout << "on_close" << endl;
    if (handle != NULL)
    {
        delete handle;
    }
}

void NetHandler::on_new_connection(uv_stream_t* server, int status)
{
    //cerr << "on_new_connection " << status << endl;
    IUvSocket* uvSocket = (IUvSocket*)server->data;
    //cerr << "on_new_connection " << status << " " << (long)uvSocket << endl;
    if (uvSocket == nullptr)
    {
        cerr << "Fatal error: uvSocket is nullptr " << (long)server->accepted_fd;
        //uv_close((uv_handle_t*)server, NULL);
        //delete server;
        return;
    }
    uvSocket->onNewConnection(server, status);
    //delete server
}

void NetHandler::onNewConnection(uv_stream_t* server, int status)
{
    cerr << "NetHandler::onNewConnection " << status << endl;
    if (status < 0)
    {
        cerr << "New connection error " << uv_strerror(status) << endl;
        //delete server;
        return;
    }

    uv_tcp_t* client = new uv_tcp_t();
    ::uv_tcp_init(getUvLoop(), client);
    int res = ::uv_accept(server, (uv_stream_t*)client);
    if (res < 0)
    {
        cerr << "Accept error " << uv_strerror(status) << endl;
        //delete server;
        return;
    }
    //cout << "accept res " << res << endl;
    string clientAddr = getRemoteAddress(client);
    //{
    //    uv_os_fd_t fd;
    //    ::uv_fileno((uv_handle_t*)client, &fd);
    //    cout << "Accepted connection " << fd << " from " << clientAddr << endl;
    //}
    shared_ptr<NetClientIn> cliin = make_shared<NetClientIn>(myPeboNet.get(), client, clientAddr);
    shared_ptr<NetClientBase> cliba = dynamic_pointer_cast<NetClientBase>(cliin);
    shared_ptr<IPeboPeer> cli = dynamic_pointer_cast<IPeboPeer>(cliin);
    assert(myPeboNet != nullptr);
    myPeboNet->inConnectionReceived(clientAddr, cli);
    int error = cliba->doRead();
    // ignore error
    // TODO myPeboNet->removePeer()
}

/*
int NetHandler::doBindPort(int port_in)
{
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        // error opening socket
        return socket;
    }

    struct sockaddr_in serv_addr;
    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_in);

    // Now bind the host address using bind() call.
    int bindRes = ::bind(socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (bindRes < 0) 
    {
        // error on binding
        cerr << "Could not bind to port " << port_in << ", probably taken " << bindRes << endl;
        return bindRes;
    }
    
    cerr << "Bound to listen port " << port_in << endl;
    return socket;
}
*/

string NetHandler::getRemoteAddress(const uv_tcp_t* socket_in)
{
    string addr("?");
    //uv_os_fd_t fd;
    //uv_fileno((uv_handle_t*)socket_in, &fd);
    struct sockaddr sockaddr;
    int addrlen = sizeof(sockaddr);
    int res = ::uv_tcp_getpeername(socket_in, &sockaddr, &addrlen);
    if (res != 0)
    {
        return addr;
    }
    if (sockaddr.sa_family == AF_INET)
    {
        char remoteIp[256];
        //sockaddr.sin_addr.s_addr = ::ntohl(sockaddr.sin_addr.s_addr);
        if (::uv_inet_ntop(AF_INET, &(((struct sockaddr_in*)&sockaddr)->sin_addr), remoteIp, 256))
        {
            return addr;
        }
        addr = string(remoteIp) + ":" + to_string(((struct sockaddr_in*)&sockaddr)->sin_port);
        return addr;
    }
    if (sockaddr.sa_family == AF_INET6)
    {
        char remoteIp[256];
        if (::uv_inet_ntop(AF_INET6, &(((struct sockaddr_in6*)&sockaddr)->sin6_addr), remoteIp, 256))
        {
            return addr;
        }
        addr = string(remoteIp) + ":" + to_string(((struct sockaddr_in6*)&sockaddr)->sin6_port);
        return addr;
    }
    return addr;
}

int NetHandler::doBindAndListen(int port_in)
{
    //cerr << "doBindAndListen trying port " << port_in << endl;
    uv_loop_t* loop = getUvLoop();
    uv_tcp_t* server = new uv_tcp_t();
    ::uv_tcp_init(loop, server);

    struct sockaddr_in addr;
    ::uv_ip4_addr("0.0.0.0", port_in, &addr);

    int res = ::uv_tcp_bind(server, (const struct sockaddr*)&addr, 0);
    if (res)
    {
        cerr << "Bind error " << uv_strerror(res) << endl;
        return res;
    }
    server->data = (void*)dynamic_cast<IUvSocket*>(this);
    res = ::uv_listen((uv_stream_t*)server, 10, NetHandler::on_new_connection);
    if (res)
    {
        cerr << "Listen error " << uv_strerror(res) << endl;
        return res;
    }
    return 0;
}

int NetHandler::doListen(int port_in, int tryNextPorts_in)
{
    int nextPorts = std::max(std::min(tryNextPorts_in, 10), 1);
    int actualPort = -1;

    for (int i = 0; i < nextPorts; ++i)
    {
        int port = port_in + i;
        int res = doBindAndListen(port);
        if (res == 0)
        {
            // we are bound and listening
            actualPort = port;
            assert(myPeboNet != nullptr);
            myPeboNet->listenStarted(actualPort);
            cerr << "Listening on " << actualPort << endl;
            return actualPort;
        }
    }
    // could not bind anywhere
    return -1;
}

void NetHandler::on_walk(uv_handle_t* handle, void* arg)
{
    //cout << "on_walk" << endl;
    ::uv_close(handle, NetHandler::on_close);
}

int NetHandler::doBgThread()
{
    //cerr << "UV LOOPLOOP starting" << endl;
    // Note: this second loop is not really necessary
    while (!myBgThreadStop)
    {
        runLoop();
        if (myBgThreadStop) break;
        //cout << "Sleeping..." << endl;
        std::this_thread::sleep_for(1s);
    }

    //cerr << "UV LOOPLOOP closed" << endl;
    return 0;
}
