#pragma once

#include "../../peerboot/shell.hpp"
#include <memory>
#include <vector>

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
        std::vector<std::shared_ptr<Shell>> myShells;
    };
}
