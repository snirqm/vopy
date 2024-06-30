#pragma once

#include "cli.h"
#include "client.h"


struct VOpyClientConfig {
    int port = -1;
    std::string host = "";
    VOpyClientConfig(int argc, char **argv);
    int parse(int argc, char **argv);
    CLI::App app{"VOPY SIMULATION SERVER"};
    boost::asio::ip::tcp::endpoint endpoint;
};

class VOpyClientApp : CLI::App {
private:
    std::unique_ptr<VOpyClientConfig> config;
    std::unique_ptr<VOpyTcpClient> client;
public:
    VOpyClientApp(int argc, char **argv);
    int run();
};


