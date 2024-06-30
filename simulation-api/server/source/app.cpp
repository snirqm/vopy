#include "app.h"

#define PORT_FLAG "-p,--port"

#define SIM_PATH_FLAG "-s,--sim-path"


// namespace utils {
//     template<typename T, typename... Args>
//     std::unique_ptr<T> make_unique(Args&&... args) {
//         return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//     }
// }

VOpyServerConfig::VOpyServerConfig(int argc, char **argv) {
    argv = app.ensure_utf8(argv);
    app.add_option(PORT_FLAG, port, "Server port")->required();
    app.add_option(SIM_PATH_FLAG, sim_path, "Path to simulation running directory, default is current directory")->capture_default_str();
    if(parse(argc, argv) != 0) {
        throw std::runtime_error("Failed to parse arguments");
    }
}

int VOpyServerConfig::parse(int argc, char **argv) {
    CLI11_PARSE(app, argc, argv)   
    return 0;
}

VOpyServerApp::VOpyServerApp(int argc, char **argv) {
    config = std::make_unique<VOpyServerConfig>(argc, argv);
    server = std::make_unique<TcpSimulationServer>(config->port, config->sim_path);
}

int VOpyServerApp::run() {
    server->start();
    return 0;
}
