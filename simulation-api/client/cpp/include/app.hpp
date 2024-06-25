#pragma once

#include "cli.hpp"
#include "client.hpp"

enum class VOpyMode {
    FIFO,
    TCP
};

struct VOpyConfig {
    CLI::App app = CLI::App{"VOPY SIMULATION SERVER"};
    int parse(int argc, char **argv);
    [[nodiscard]] VOpyMode mode() const;
};

class VOpyClientApp : CLI::App {
private:
    std::unique_ptr<VOpyConfig> config;
    std::unique_ptr<VOpyClient> client;
public:
    VOpyClientApp(int argc, char **argv);
    int run();
};


