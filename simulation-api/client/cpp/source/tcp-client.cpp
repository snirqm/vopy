#include "client.h"
#include <utility>
#include <iostream>
#include <boost/asio.hpp>

VOpyTcpClient::VOpyTcpClient(boost::asio::ip::tcp::endpoint endpoint)
        : endpoint(std::move(endpoint)), socket(io_context) {
}

void VOpyTcpClient::connect() {
    socket.connect(endpoint);
}

void VOpyTcpClient::disconnect() {
    socket.close();
}

void VOpyTcpClient::send_command(const VOpyCommand &cmd) {
    std::string message = cmd_to_string(cmd);
    boost::asio::write(socket, boost::asio::buffer(message + "\n"));
}

VOpyCommandResult VOpyTcpClient::receive_result() {
    boost::asio::streambuf request;
    boost::asio::read_until(socket, request, "\n");
    std::string message = std::string(std::istreambuf_iterator<char>(&request), {});
    return result_from_string(message.c_str());
}

