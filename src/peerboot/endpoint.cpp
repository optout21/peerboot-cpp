#include "endpoint.hpp"

#include <iostream>

using namespace pebo;
using namespace std;

Endpoint::Endpoint(string endpointStr_in)
{
    myPort = 0;
    // parse by colon
    auto colonIdx = endpointStr_in.find(':');
    if (colonIdx == string::npos)
    {
        // no colon
        myHost = endpointStr_in;
        return;
    }
    // there is a colon
    myHost = endpointStr_in.substr(0, colonIdx);
    try
    {
        myPort = std::stoi(endpointStr_in.substr(colonIdx + 1, endpointStr_in.length() - colonIdx - 1));
    }
    catch(const std::exception& e)
    {
        myPort = 0;
    }        
}
