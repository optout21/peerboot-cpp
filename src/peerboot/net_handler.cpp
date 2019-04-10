#include "net_handler.hpp"

#include "ipebo_net.hpp"
#include "net_client.hpp"

#include <cassert>
#include <iostream>

#include <arpa/inet.h> // inet_ntop
#include <netdb.h>
#include <netinet/in.h>
#include <string.h> // bzero
#include <unistd.h>

using namespace pebo;
using namespace std;

void NetHandler::init(std::shared_ptr<IPeboNet> peboNet_in)
{
    myPeboNet = peboNet_in;
}

errorCode NetHandler::start(int port_in, int tryNextPorts_in)
{
    int actualPort = doListen(port_in, tryNextPorts_in);
    if (actualPort < 0)
    {
        return errorCode::err_networkListen;
    }

    // we don't know our external endpoint, use value with local hostname and port
    string nodeId = getHostIp() + ":" + to_string(actualPort);
    myPeboNet->setNodeId(nodeId);

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

errorCode NetHandler::stop()
{
    // TODO wake up, unbind
    myBgThreadStop = true;
    myBgThread.join();
    return errorCode::err_ok;
}

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

int NetHandler::doListen(int port_in, int tryNextPorts_in)
{
    int nextPorts = std::max(std::min(tryNextPorts_in, 10), 1);
    int actualPort = -1;
    for (int i = 0; i < nextPorts; ++i)
    {
        int port = port_in + i;
        int listenSoc = doBindPort(port);
        if (listenSoc < 0)
        {
            continue;
        }
        // we are bound
        myListenSocket = listenSoc;
        actualPort = port;
        break;
    }
    
    // start listening
    int listenRes = ::listen(myListenSocket, 5);
    assert(listenRes > 0);
    cerr << "Listening" << endl;

    myBgThread = move(thread([=]() { return this->doBgThread(); }));
    
    return actualPort;
}

int NetHandler::doBgThread()
{
    while (!myBgThreadStop)
    {
        // accept, block until an incoming connection
        // TODO proper waking up
        struct sockaddr_in cli_addr;
        size_t clilen = sizeof(cli_addr);
        bzero((char *) &cli_addr, sizeof(cli_addr));
        //cerr << "accepting..." << endl;
        int clientSocket = ::accept(myListenSocket, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen);
        if (clientSocket < 0)
        {
            // error in accept
            return clientSocket;
        }
        char remoteIp[256];
        //cli_addr.sin_addr.s_addr = ::ntohl(cli_addr.sin_addr.s_addr);
        if (::inet_ntop(AF_INET, &cli_addr.sin_addr, remoteIp, 256) == NULL)
        {
            // error with remote address
            return -1;
        }
        string nodeId = string(remoteIp) + ":" + to_string(cli_addr.sin_port);
        //cerr << "Accepted incoming connection remote " << nodeId << " socket " << clientSocket << " ip " << remoteIp << " port " << cli_addr.sin_port << endl;
        auto cli = make_shared<NetClientIn>(myPeboNet.get(), clientSocket, nodeId);
        myPeboNet->addPeer(nodeId, cli);
        errorCode error = cli->doProcessIncomingMessage();
        // ignore error
        // TODO myPeboNet->removePeer()
    }

    return 0;
}

string NetHandler::getHostIp()
{
    string ip = "LOCAL";
    char hostname[256];
    if (::gethostname(hostname, sizeof(hostname)) != 0) return ip;
    struct hostent* hostent = ::gethostbyname(hostname);
    if (hostent->h_length <= 0) return ip;
    //if (hostent->h_addr_list[0] == 0) return ip;
    for (int i = 0; i < hostent->h_length; ++i)
    {
        char ipStr[256];
        if (::inet_ntop(hostent->h_addrtype, hostent->h_addr_list[i], ipStr, 256) != NULL)
        {
            ip = ipStr;
            break;
        }
    }
    return ip;
}
