#include "uv_socket.hpp"

#include <string.h> // memcpy

using namespace pebo;

UvWriteRequest::UvWriteRequest(IUvSocket* uvSocket_in, int nbuf_in) :
uvSocket(uvSocket_in),
nbuf(0), // set later
bufs(new uv_buf_t[nbuf_in])
{
}

UvWriteRequest::~UvWriteRequest()
{
    for (int i = 0; i < nbuf; ++i)
    {
        delete[] bufs[i].base;
    }
    delete[] bufs;
}

void UvWriteRequest::add(std::vector<uint8_t> const & buf_in)
{
    size_t bufsize = buf_in.size();
    uint8_t* buf2 = new uint8_t[bufsize];
    ::memcpy(buf2, &buf_in[0], bufsize);
    uv_buf_t uvbuf = ::uv_buf_init((char*)&buf2[0], bufsize);
    bufs[nbuf] = uvbuf;
    ++nbuf;
}
