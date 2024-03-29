#include "../../include/peerboot.hpp"
#include <iostream>
#include <ctime>

using namespace std;

void notificationCB(pebo::PeerInfo self_in, pebo::PeerInfo peer_in)
{
    cout << "Notification: " << (peer_in.isRemoved ? "Removed" : "new    ") << " " << peer_in.service << " " << peer_in.endpoint << " " << peer_in.lastSeen << endl;
}

// Generate endpoint suffix based on current time
string getEndpointSuffix()
{
    std::string suffix;
    // get current time
    std::time_t t = std::time(0);
    std::tm* nowtm = std::localtime(&t);
    string asctime = std::asctime(nowtm);
    if (asctime.length() >= 19) suffix = asctime.substr(11, 8); // HH:MM:SS
    return suffix;
}

int main()
{
    cout << "Sample PeerBoot Client, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;

    pebo::service_t service ("sample.peerboot.io");
    pebo::endpoint_t endpoint ("dummy_endpoint_" + getEndpointSuffix());

    pebo::errorCode err = pebo::init(service, endpoint, ::notificationCB);
    if (err)
    {
        cerr << "Could not initialize PeerBoot library, err: " << err << " service: " << service << " endpoint: " << endpoint << endl;
        return err;
    }
    cout << "PeerBoot library initialized, service: " << service << " endpoint: " << endpoint << endl;

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    err = pebo::deinit();
    cout << "PeerBoot library deinitialized, err: " << err << endl;
}
