#pragma once

#include "cli.hpp"
#include "server.hpp"

enum class VOpyMode {
    FIFO,
    TCP
};

struct VOpyConfig {
    CLI::App app = CLI::App{"VOPY SIMULATION SERVER"};
    int parse(int argc, char **argv);
    [[nodiscard]] VOpyMode mode() const;
};

class VOpyApp : CLI::App {
private:
    std::unique_ptr<VOpyConfig> config;
    std::unique_ptr<VOpySimulationServer> server;
public:
    VOpyApp(int argc, char **argv);
    int run();
};


