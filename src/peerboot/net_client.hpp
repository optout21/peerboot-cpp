#pragma once

#include "ipebo_peer.hpp"
//#include "ipebo_net.hpp"
//#include "ipebo_peer_cb.hpp"
#include "uv_socket.hpp"

#include <uv.h>

#include <queue>
#include <string>
#include <vector>

namespace pebo
{
    class IPeboNet; // forward
    class IPeboPeerCB; // forward

    /**
     * Network PeerBoot client.  Simple Clib networking.
     */
    class NetClientBase: public IPeboPeer
    {
    public:
        NetClientBase(IPeboNet* peboNet_in, std::string const & nodeId_in);
        void setNotifyCB(IPeboPeerCB* peboPeerCB_in);
        std::string getNodeId() const { return myNodeId; }
 
    protected:
        IPeboNet* myPeboNet;
        std::string myNodeId;
    };

    /**
     * Network PeerBoot client, incoming connection.  Simple Clib networking.
     */
    class NetClientIn: public NetClientBase
    {
    public:
        NetClientIn(IPeboNet* peboNet_in, int socket_in, std::string const & nodeId_in);
        // Send a message to this peer
        errorCode sendMsg(BaseMessage const & msg_in);
        errorCode doProcessIncomingMessage();

    private:
        int mySocket;
    };

    /**
     * Network PeerBoot client, outgoing connection.  Simple Clib networking.
     */
    class NetClientOut: public IUvSocket, public NetClientBase
    {
    public:
        NetClientOut(IPeboNet* peboNet_in, std::string const & host_in, int port_in);
        // Send a message to this peer
        errorCode sendMsg(BaseMessage const & msg_in);
        static void on_connect(uv_connect_t* req, int status);
        static void on_write(uv_write_t* req, int status);
        void onConnect(uv_connect_t* req, int status);
        void onWrite(uv_write_t* req, int status);
        void doSend(uv_stream_t* handle);

    private:
        std::string myHost;
        int myPort;
        std::queue<std::vector<uint8_t>> mySendQueue;
    };
}
