#pragma once

#include <uv.h>
#include <vector>

namespace pebo
{
    /*
     * A class reacting to socket events.
     * Not really an interface, as empty implemetations are contained for convenience.
     */
    class IUvSocket
    {
    public:
        virtual void onConnect(uv_connect_t* req, int status) { }
        virtual void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) { }
        virtual void onWrite(uv_write_t* req, int status) { }
        virtual void onNewConnection(uv_stream_t* server, int status) { }
        virtual void onClose(uv_handle_t* handle) { }
        virtual void onTimer(uv_timer_t* timer) { }
    };

    /// Used together with write requests, keeps reference to write buffer while needed.
    class UvWriteRequest
    {
    public:
        typedef uint8_t* buf_t;
        IUvSocket* uvSocket;
        uv_buf_t* bufs;
        int nbuf;

    public:
        UvWriteRequest(IUvSocket* uvSocket_in, int nbuf_in);
        ~UvWriteRequest();
        void add(std::vector<uint8_t> const & buf_in);
    };
}
