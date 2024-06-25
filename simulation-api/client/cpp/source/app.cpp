#include "app.hpp"
#include <print>

#define FILE_MODE_CMD "file"
#define FILE_MODE_IN_FLAG "-i"
#define FILE_MODE_OUT_FLAG "-o"

#define TCP_MODE_CMD "tcp"
#define TCP_MODE_PORT_FLAG "-p"

int VOpyConfig::parse(int argc, char **argv) {
    argv = app.ensure_utf8(argv);
    auto fifo_server = app.add_subcommand(FILE_MODE_CMD, "fifo file server mode");
    fifo_server->add_option(FILE_MODE_IN_FLAG, "Input FIFO file name");
    fifo_server->add_option(FILE_MODE_OUT_FLAG, "Output FIFO file name");

    auto tcp_server = app.add_subcommand(TCP_MODE_CMD, "TCP server mode");
    tcp_server->add_option(TCP_MODE_PORT_FLAG, "Server port");
    CLI11_PARSE(app, argc, argv)
    return 0;
}

[[nodiscard]] VOpyMode VOpyConfig::mode() const {
    if (app.get_subcommand("file")->parsed()) {
        return VOpyMode::FIFO;
    } else if (app.get_subcommand("tcp")->parsed()) {
        return VOpyMode::TCP;
    } else {
        throw std::runtime_error("Invalid mode");
    }
}

VOpyClientApp::VOpyClientApp(int argc, char **argv) : config(std::make_unique<VOpyConfig>()), client() {
    config->parse(argc, argv);
    switch (config->mode()) {
        case VOpyMode::FIFO: {
            auto sub_command = config->app.get_subcommand(FILE_MODE_CMD);
            client = std::make_unique<FileSimulationClient>(
                    sub_command->get_option(FILE_MODE_IN_FLAG)->as<std::string>(),
                    sub_command->get_option(FILE_MODE_OUT_FLAG)->as<std::string>());
            break;
        }
        case VOpyMode::TCP: {
            auto sub_command = config->app.get_subcommand(TCP_MODE_CMD);
            client = std::make_unique<TcpSimulationClient>(sub_command->get_option(TCP_MODE_PORT_FLAG)->as<int>());
            break;
        }
    }
}

int VOpyClientApp::run() {
    client->connect();
    return 0;
}
