#include "net_client.hpp"
#include "ipebo_net.hpp"
#include "message.hpp"

#include <cassert>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace pebo;
using namespace std;


NetClientBase::NetClientBase(IPeboNet* peboNet_in, string const & nodeId_in) :
myPeboNet(peboNet_in),
myNodeId(nodeId_in)
{
}


void NetClientBase::setNotifyCB(IPeboPeerCB* peboPeerCB_in)
{
    assert(false);
    // use myPeboNet;
}


NetClientIn::NetClientIn(IPeboNet* peboNet_in, int socket_in, string const & nodeId_in) :
NetClientBase(peboNet_in, nodeId_in),
mySocket(socket_in)
{
}

errorCode NetClientIn::sendMsg(BaseMessage const & msg_in)
{
    cerr << "ERROR NetClientIn::sendMsg " << myNodeId << " " << msg_in.getType() << endl;
    assert(false);
    // TODO
    return errorCode::err_generic;
}

errorCode NetClientIn::doProcessIncomingMessage()
{
    assert(mySocket > 0);
    // communicate with client, process one message
    static const int buflen = 256;
    char buffer[buflen];
    bzero(buffer, buflen);
    int n = ::read(mySocket, buffer, buflen );
    if (n < 0)
    {
        cerr << "Error reading from socket " << n << " " << mySocket << endl;
        ::close(mySocket);
        return errorCode::err_generic;
    }
    if (n > 0)
    {
        string messageRaw(buffer);
        cerr << "Incoming message: " << myNodeId << " '" << messageRaw << "'" << endl;
        if (messageRaw.length() > 5 && messageRaw.substr(0, 5) == "PEER ")
        {
            string rest = messageRaw.substr(5);
            int spaceIdx = rest.find(' ');
            //cerr << rest << " " << spaceIdx << endl;
            if (spaceIdx > 0)
            {
                string word1 = rest.substr(0, spaceIdx);
                string word2 = rest.substr(spaceIdx + 1);
                //cerr << "words " << word1 << " " << word2 << " " << myPeboNet << endl;
                assert(myPeboNet != nullptr);
                myPeboNet->msgFromPeboPeer(myNodeId, PeerUpdateMessage(word1, word2, 0, false, 0));
            }
        }
        else if (messageRaw.length() > 6 && messageRaw.substr(0, 6) == "QUERY ")
        {
            string service = messageRaw.substr(6);
            assert(myPeboNet != nullptr);
            myPeboNet->msgFromPeboPeer(myNodeId, QueryMessage(service, 0));
        }

        /*
        // Write a dummy response
        n = ::write(mySocket,"ok", 2);
        if (n < 0)
        {
            cerr << "Error writing to socket" << endl;
            return errorCode::err_generic;
        }
        */
    }

    ::close(mySocket);
    mySocket = EINVAL;

    return errorCode::err_ok;
}

class SendMessageVisitor: public MessageVisitorBase
{
public:
    SendMessageVisitor(NetClientOut & client_in) :
        MessageVisitorBase(), myClient(client_in) { }
    virtual ~SendMessageVisitor() = default;
    void peerUpdate(PeerUpdateMessage const & msg_in);
    void query(QueryMessage const & msg_in);
    string getMessage() const { return myMessage; }

private:
    NetClientOut & myClient;
    std::string myNodeId;
    string myMessage;
};

void SendMessageVisitor::peerUpdate(PeerUpdateMessage const & msg_in)
{
    myMessage = "PEER " + msg_in.getService() + " " + msg_in.getEndpoint();
}

void SendMessageVisitor::query(QueryMessage const & msg_in)
{
    cerr << "SendMessageVisitor::query " << msg_in.getService() << endl;
    myMessage = "QUERY " + msg_in.getService();
}


NetClientOut::NetClientOut(IPeboNet* peboNet_in, string const & host_in, int port_in) :
NetClientBase(peboNet_in, host_in + ":" + to_string(port_in)),
myHost(host_in),
myPort(port_in)
{
}

errorCode NetClientOut::sendMsg(BaseMessage const & msg_in)
{
    // convert to message
    SendMessageVisitor visitor(*this);
    msg_in.visit(visitor);
    string msg = visitor.getMessage();

    // send message   
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        // error opening socket
        return errorCode::err_networkGeneric;
    }

    struct hostent* server = ::gethostbyname(myHost.c_str());
    if (server == NULL)
    {
        // no such host
        return errorCode::err_networkGeneric;
    }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(myPort);
    // Now connect to the server
    int connRes = ::connect(socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (connRes < 0)
    {
        // error connecting
        return errorCode::err_networkGeneric;
    }
    //cerr << "NetClientOut: " << myNodeId << " Connected to remote peer " << myNodeId << endl;

    // Send message
    int written = ::write(socket, msg.c_str(), strlen(msg.c_str()));
    if (written < 0)
    {
        // error writing to socket
        return errorCode::err_networkGeneric;
    }

    ::close(socket);

    return errorCode::err_ok;
}
