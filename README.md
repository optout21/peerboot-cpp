# PeerBoot
## What is PeerBoot?
PeerBoot is a library _and_ P2P network that solves initial peer node discovery for any P2P project.

This is the C++ reference implementation.

## Why PeerBoot?
* No need to set up and maintain own infrastructure for bootstrap nodes
* Synergy between different P2P projects -- PeerBoot insfrastructure is shared by different P2P projects
* Natural way for blockchain/crypto/P2P development is specialization, but not towards libraries, but other P2P sub-services
* Fast and reliable startup.  Within a small P2P projects with small number of nodes, PeerBoot bootstrap is reliable, due to the many PeerBoot nodes.
* Lightweight, fast, it does one thing, and uses limited resources

## Status
Early prototyping phase.

## Build instructions

### Prerequisites
* C++ compiler
* CMake (min 3.10)
* LibUV (libuv1-dev)

### Obtain sources
git clone https://github.com/catenocrypt/peerboot-cpp.git

### Initalize CMake and compile
cmake -DCMAKE_BUILD_TYPE=Debug .

make

### Sample app
./peerboot_sampleclientapp


