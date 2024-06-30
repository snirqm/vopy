#pragma once

#include "cli.h"
#include "server.h"
#include "easylogging++.h"

struct VOpyServerConfig {
    CLI::App app{"VOPY SIMULATION SERVER"};
    int port = -1;
    std::string sim_path = ".";
    VOpyServerConfig(int argc, char **argv);
    int parse(int argc, char **argv);
};

class VOpyServerApp : CLI::App {
private:
    std::unique_ptr<VOpyServerConfig> config;
    std::unique_ptr<TcpSimulationServer> server;
public:
    VOpyServerApp(int argc, char **argv);
    int run();
};


