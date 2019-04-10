#pragma once

#include "../../peerboot/shell.hpp"
#include <memory>
#include <map>

namespace pebo
{
    /**
     * Testbench for connected shells
     */
    class TestBenchShells
    {
    public:
        TestBenchShells() = default;
        void addShell(std::shared_ptr<Shell> shell_in);
        void connect();
        void deinit();

    private:
        std::map<std::string, std::shared_ptr<Shell>> myShells;
        static long myCounter;
    };
}
