#pragma once

#include "cli.h"
#include "client.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;


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

    VOpyCommand read_command_from_stdin();
public:
    VOpyClientApp(int argc, char **argv);
    int run();
};


