#include <string>
#include <boost/asio.hpp>
#include "server.h"

using boost::asio::ip::tcp;


VOpyCommandResult TcpSimulationServer::execute(const VOpyCommand &cmd) {
    VOpyCommandResult result{};
    switch (cmd.type) {
        case READ:
            execute_read(cmd.read.address, cmd.read.num_of_bytes, &result);
            break;
        case WRITE:
            execute_write(cmd.write.address, cmd.write.data, cmd.write.num_of_bytes, &result);
            break;
        case SIM_CLOCK:
            execute_sim_clock(cmd.sim_clock.cycles, &result);
            break;
        case SIM_TO_LAST:
            execute_sim_to_last(&result);
            break;
        case EXIT:
            execute_exit(&result);
            exit(0);
        case UNKNOWN:
            break;
    }
    result.id = cmd.id;
    return result;
}

void TcpSimulationServer::start() {
    while (true) {
        try {
            auto response = execute(receive_command());
            if (!response.empty) send_result(response);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}


TcpSimulationServer::TcpSimulationServer(int port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
                                                     socket(io_context) {
    acceptor.accept(socket);
}

VOpyCommand TcpSimulationServer::receive_command() {
    boost::asio::streambuf request;
    run_or_accept<void>([&]() {
        boost::asio::read_until(socket, request, "\n");
    });
    std::string message = std::string(std::istreambuf_iterator<char>(&request), {});
    return VOpyCommand{cmd_from_string(message.c_str())};
}

void TcpSimulationServer::send_result(const VOpyCommandResult &res) {
    std::string message = result_to_string(res);
    run_or_accept<void>([&]() {
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));
    });
}
