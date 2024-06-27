#include "app.h"
#include "easylogging++.h"

#include <print>

#define PORT_FLAG "-p"
#define HOST_FLAG "-i"

VOpyClientConfig::VOpyClientConfig(int argc, char **argv) {
    argv = app.ensure_utf8(argv);
    app.add_option(HOST_FLAG, host, "Server host");
    app.add_option(PORT_FLAG, port, "Server port");
    parse(argc, argv);
}

int VOpyClientConfig::parse(int argc, char **argv) {
    CLI11_PARSE(app, argc, argv)
    LOG(INFO) << "Host: " << host;
    LOG(INFO) << "Port: " << port;
    endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port);
    return 0;
}


VOpyClientApp::VOpyClientApp(int argc, char **argv) {
    config = std::make_unique<VOpyClientConfig>(argc, argv);
    client = std::make_unique<VOpyTcpClient>(config->endpoint);
}

int VOpyClientApp::run() {
    int count = 0;
    client->connect();
    while (true) {
        std::string cmd_str;
        std::getline(std::cin, cmd_str);
        LOG(INFO) << "Sending command: " << cmd_str;
        if (cmd_str == "exit") {
            break;
        }
        auto cmd = cmd_from_string(cmd_str.c_str());
        client->send_command(cmd);
        auto result = client->receive_result();
        if (result.empty) {
            LOG(INFO) << "Empty result";
        } else if (result.ok) {
            LOG(INFO) << "OK result";
        } else {
            switch (result.type) {
                case READ:
                    LOG(INFO) << "Read result: " << result.result.read.value;
                    break;
                default:
                    LOG(INFO) << "Unknown result";
            }
        }
    }
    return 0;
}
