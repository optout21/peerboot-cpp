#pragma once

#include "inet_handler.hpp"
//#include "pebo_net.hpp"
#include "uv_socket.hpp"

//#include <uv.h>

#include <memory>
#include <string>
#include <thread>

namespace pebo
{
    class IPeboNet; // forward
    typedef struct uv_loop_s uv_loop_t; // forward

    class NetHandler: public IUvSocket, public INetHandler
    {
    public:
        NetHandler() = default;

    public:
        // UV loop handling
        static uv_loop_t* getUvLoop();
        static void deleteUvLoop();
        static int runLoop();

        void init(std::shared_ptr<IPeboNet> peboNet_in);
        /// Return actual listen port
        int start(int port_in, int tryNextPorts_in);
        int stop();
        void onNewConnection(uv_stream_t* server, int status);
        static std::string getRemoteAddress(const uv_tcp_t* socket_in);

    private:
        int startUvLoop();
        int stopUvLoop();
        int doBindAndListen(int port_in);
        // returns the actual listen port, or <0 for error
        int doListen(int port_in, int tryNextPorts_in);
        int doBgThread();
        static void on_new_connection(uv_stream_t* server, int status);
        static void on_close(uv_handle_t* handle);
        static void on_walk(uv_handle_t* handle, void* arg);

    private:
        static uv_loop_t* myUvLoop;
        std::shared_ptr<IPeboNet> myPeboNet;
        uv_async_t* myUvAsync;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}
