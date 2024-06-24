#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

using boost::asio::ip::tcp;

//
// TcpSimulationServer
//

TcpSimulationServer::TcpSimulationServer(int port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
                                                     socket(io_context) {
    acceptor.accept(socket);
}

Command TcpSimulationServer::receive_command() {
    boost::asio::streambuf request;
    run_or_accept<void>([&]() {
        boost::asio::read_until(socket, request, "\n");
    });
    std::string message = std::string(std::istreambuf_iterator<char>(&request), {});
    return Command{from_string(message)};
}

void TcpSimulationServer::send_result(const struct CommandResult &res) {
    std::string message = to_string(res);
    run_or_accept<void>([&]() {
        boost::asio::write(socket, boost::asio::buffer(message));
    });
}
