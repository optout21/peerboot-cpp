#include "src/peerboot/peerboot.hpp"

#include <iostream>

using namespace pebo;
using namespace std;

int main()
{
    cout << "Sample PeerBoot Client, v" << PEBO_VERSION_MAJOR << "." << PEBO_VERSION_MINOR << endl;

    peerboot_lib();
}
