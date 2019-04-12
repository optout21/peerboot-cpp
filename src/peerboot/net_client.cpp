#include "net_client.hpp"
#include "ipebo_net.hpp"
#include "message.hpp"
#include "uv_socket.hpp"

#include <uv.h>

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
    string pending;
    static const int buflen = 256;
    char buffer[buflen];
    while (true)
    {
        bzero(buffer, buflen);
        int n = ::read(mySocket, buffer, buflen );
        if (n < 0)
        {
            cerr << "Error reading from socket " << n << " " << mySocket << " pending " << pending.length() << endl;
            ::close(mySocket);
            mySocket = EINVAL;
            return errorCode::err_generic;
        }
        if (n == 0)
        {
            cerr << "Socket closed while reading " << mySocket << " pending " << pending.length() << endl;
            ::close(mySocket);
            mySocket = EINVAL;
            return errorCode::err_ok;
        }
        pending.append(buffer);
        //cerr << "read " << n << " bytes" << " pending " << pending.length() << endl;
        // process accumulated pending
        int terminatorIdx;
        while ((terminatorIdx = pending.find('\n')) >= 0)
        {
            string msg1 = pending.substr(0, terminatorIdx); // without the terminator
            pending = pending.substr(terminatorIdx + 1);
            cerr << "Incoming message: " << myNodeId << " '" << msg1 << "'" << endl;
            if (msg1.length() > 5 && msg1.substr(0, 5) == "PEER ")
            {
                string rest = msg1.substr(5);
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
            else if (msg1.length() > 6 && msg1.substr(0, 6) == "QUERY ")
            {
                string service = msg1.substr(6);
                assert(myPeboNet != nullptr);
                myPeboNet->msgFromPeboPeer(myNodeId, QueryMessage(service, 0));
            }
        }
    }

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
    myMessage = "QUERY " + msg_in.getService();
}


NetClientOut::NetClientOut(IPeboNet* peboNet_in, string const & host_in, int port_in) :
NetClientBase(peboNet_in, host_in + ":" + to_string(port_in)),
myHost(host_in),
myPort(port_in)
{
}


void NetClientOut::on_write(uv_write_t* req, int status) 
{
    UvWriteBaton* baton = (UvWriteBaton*)req;
    if (baton == nullptr || baton->uvSocket == nullptr)
    {
        cerr << "Fatal error: UvWriteBaton.uvSocket is nullptr " << (long)req->handle;
        uv_close((uv_handle_t*)req->handle, NULL);
        return;
    }
    baton->uvSocket->onWrite(req, status);
    delete baton;
}

void NetClientOut::onWrite(uv_write_t* req, int status) 
{
    if (status != 0) 
    {
        cerr << "write error " << myHost << ":" << myPort << " " << status << " " << uv_strerror(status) << endl;
        uv_close((uv_handle_t*) req->handle, NULL);
        return;
    }
    doSend(req->handle);
}

void NetClientOut::on_connect(uv_connect_t* req, int status)
{
    //cerr << "on_connect " << status << " " << req->type << endl;
    UvConnectBaton* baton = (UvConnectBaton*)req;
    if (baton == nullptr || baton->uvSocket == nullptr)
    {
        cerr << "Fatal error: UvConnectBaton.uvSocket is nullptr " << (long)req->handle;
        uv_close((uv_handle_t*)req->handle, NULL);
        return;
    }
    baton->uvSocket->onConnect(req, status);
    delete baton;
}

void NetClientOut::doSend(uv_stream_t* handle)
{
    // local copy
    vector<vector<uint8_t>> sending;
    while (!mySendQueue.empty())
    {
        sending.push_back(mySendQueue.front());
        mySendQueue.pop();
    }
    //cerr << "doSend " << sending.size() << " buffers" << endl;
    if (sending.empty())
    {
        // no more to send, close
        uv_close((uv_handle_t*)handle, NULL);
        return;
    }
    int n = sending.size();
    UvWriteBaton* wrbat = new UvWriteBaton(this, n);
    for (auto i = 0; i < n; ++i)
    {
        wrbat->add(sending[i]);
    }
    uv_write((uv_write_t*)wrbat, handle, &(wrbat->bufs[0]), wrbat->nbuf, NetClientOut::on_write);
}

void NetClientOut::onConnect(uv_connect_t* req, int status)
{
    if (status != 0) 
    {
        cerr << "connect error " << myHost << ":" << myPort << " " << status << " " << uv_strerror(status) << endl;
        uv_close((uv_handle_t*) req->handle, NULL);
        return;
    }
    doSend(req->handle);
}

errorCode NetClientOut::sendMsg(BaseMessage const & msg_in)
{
    // convert to message
    SendMessageVisitor visitor(*this);
    msg_in.visit(visitor);
    string msg = visitor.getMessage();
    //cerr << "sendMsg " << msg << endl;
    msg += '\n'; // terminator
    //cerr << "sendMsg " << msg.length() << " '" << msg << "'" << endl;
    // convert to byte array
    vector<uint8_t> binstr(msg.begin(), msg.end());
    mySendQueue.push(binstr);

    if (true)
    {
        //uv_loop_t* loop = (uv_loop_t*)::malloc(sizeof(uv_loop_t));
        //uv_loop_init(loop);

        uv_loop_t* loop = uv_default_loop();

        uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
        uv_tcp_init(loop, socket);

        struct sockaddr_in dest;
        uv_ip4_addr(myHost.c_str(), myPort, &dest);

        UvConnectBaton* connbat = new UvConnectBaton(this);
        uv_tcp_connect((uv_connect_t*)connbat, socket, (const struct sockaddr*)&dest, NetClientOut::on_connect);

        uv_run(loop, UV_RUN_DEFAULT);

        uv_loop_close(loop);
        //free(loop);
        //cerr << "uv loop closed" << endl;
    }

    return errorCode::err_ok;
}
