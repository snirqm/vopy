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

void VOpyTcpClient::send_command(const VOpyCommand &cmd, char *buffer) {
    void *cmd_buf = (void *) &cmd;
    int bytes_written = 0;
    while (bytes_written < sizeof(struct VOpyCommand)) {
        bytes_written += boost::asio::write(socket, boost::asio::buffer(cmd_buf + bytes_written, sizeof(struct VOpyCommand) - bytes_written));
    }
    if (cmd.type == WRITE) {
        send_data(buffer, cmd.write.num_of_bytes);
    }
}

VOpyCommandResult VOpyTcpClient::receive_result(char *buffer) {
    VOpyCommandResult result;
    void *result_buf = (void *) &result;
    int bytes_read = 0;
    while (bytes_read < sizeof(struct VOpyCommandResult)) {
        bytes_read += boost::asio::read(socket, boost::asio::buffer(result_buf + bytes_read, sizeof(struct VOpyCommandResult) - bytes_read));
    }
    if (result.type == READ) {
        receive_data(buffer, result.data_size);
    }
    return result;
}


void VOpyTcpClient::send_data(const void *data, uint32_t num_of_bytes) {
    int bytes_written = 0;
    while (bytes_written < num_of_bytes) {
        bytes_written += boost::asio::write(socket, boost::asio::buffer(data + bytes_written, num_of_bytes - bytes_written));
    }
}

void VOpyTcpClient::receive_data(void *data, uint32_t num_of_bytes) {
    int bytes_read = 0;
    while (bytes_read < num_of_bytes) {
        bytes_read += boost::asio::read(socket, boost::asio::buffer(data + bytes_read, num_of_bytes - bytes_read));
    }
}