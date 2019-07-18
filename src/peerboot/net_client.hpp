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
     * Network PeerBoot connection.
     */
    class NetClientBase: public IUvSocket, public IPeboPeer
    {
    public:
        enum State
        {
            Undefined = 0,
            NotConnected,
            Connecting,
            Connected,
            Accepted,
            Sending,
            Sent,
            Receiving,
            Received,
            Closing,
            Closed
        };

    public:
        NetClientBase(IPeboNet* peboNet_in, std::string const & nodeAddr_in);
        virtual ~NetClientBase();
        void setNotifyCB(IPeboPeerCB* peboPeerCB_in);
        std::string getNodeAddr() const { return myNodeAddr; }
        // Send a message to this peer
        int sendMessage(BaseMessage const & msg_in);
        int close();
        void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
        void onWrite(uv_write_t* req, int status);
        void onClose(uv_handle_t* handle);
        int doRead();
        virtual void process() { }
        bool isConnected() const;

    protected:
        void setUvStream(uv_tcp_t* stream_in);

    private:
        static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
        static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
        static void on_write(uv_write_t* req, int status);
        static void on_close(uv_handle_t* handle);
        void doProcessReceivedBuffer();
 
    protected:
        IPeboNet* myPeboNet;
        State myState;

    private:
        std::string myNodeAddr;
        std::string myReceiveBuffer;
        uv_tcp_t* myUvStream;
    };

    /**
     * Network PeerBoot incoming connection.
     */
    class NetClientIn: public NetClientBase
    {
    public:
        NetClientIn(IPeboNet* peboNet_in, uv_tcp_t* client_in, std::string const & nodeAddr_in);
    };

    /**
     * Network PeerBoot outgoing connection.
     */
    class NetClientOut: public NetClientBase
    {
    public:
        NetClientOut(IPeboNet* peboNet_in, std::string const & host_in, int port_in);
        int connect();
        // Perform state-dependent next action in the client state diagram
        virtual void process();
        void onConnect(uv_connect_t* req, int status);

    private:
        static void on_connect(uv_connect_t* req, int status);

    private:
        std::string myHost;
        int myPort;
    };
}
