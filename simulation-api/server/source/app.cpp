#include "app.h"
#include <print>

#define PORT_FLAG "-p"

VOpyServerConfig::VOpyServerConfig(int argc, char **argv) {
    argv = app.ensure_utf8(argv);
    app.add_option(PORT_FLAG, port, "Server port");
    parse(argc, argv);
}

int VOpyServerConfig::parse(int argc, char **argv) {
    CLI11_PARSE(app, argc, argv)
    LOG(INFO) << "Port: " << port;
    return 0;
}

[[nodiscard]] VOpyMode VOpyServerConfig::mode() const {
    if (app.get_subcommand("file")->parsed()) {
        return VOpyMode::FIFO;
    } else if (app.get_subcommand("tcp")->parsed()) {
        return VOpyMode::TCP;
    } else {
        throw std::runtime_error("Invalid mode");
    }
}

VOpyServerApp::VOpyServerApp(int argc, char **argv) {
    config = std::make_unique<VOpyServerConfig>(argc, argv);
    server = std::make_unique<TcpSimulationServer>(config->port);
}

int VOpyServerApp::run() {
    server->start();
    return 0;
}