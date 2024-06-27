#pragma once

#include "cli.h"
#include "server.h"

enum class VOpyMode {
    FIFO,
    TCP
};

struct VOpyServerConfig {
    CLI::App app = CLI::App{"VOPY SIMULATION SERVER"};
    int port = -1;
    VOpyServerConfig(int argc, char **argv);
    int parse(int argc, char **argv);
    [[nodiscard]] VOpyMode mode() const;
};

class VOpyServerApp : CLI::App {
private:
    std::unique_ptr<VOpyServerConfig> config;
    std::unique_ptr<TcpSimulationServer> server;
public:
    VOpyServerApp(int argc, char **argv);
    int run();
};


