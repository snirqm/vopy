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
    void *cmd_buf = (void *) &cmd;
    int bytes_written = 0;
    while (bytes_written < sizeof(struct VOpyCommand)) {
        bytes_written += boost::asio::write(socket, boost::asio::buffer(cmd_buf + bytes_written, sizeof(struct VOpyCommand) - bytes_written));
    }
    if (cmd.type == BIG_WRITE) {
        bytes_written = 0;
        while (bytes_written < cmd.big_write.num_of_bytes) {
            bytes_written += boost::asio::write(socket, boost::asio::buffer(cmd.big_write.data + bytes_written, cmd.big_write.num_of_bytes - bytes_written));
        }
        free(cmd.big_write.data);
    }
}

VOpyCommandResult VOpyTcpClient::receive_result() {
    VOpyCommandResult result;
    void *result_buf = (void *) &result;
    int bytes_read = 0;
    while (bytes_read < sizeof(struct VOpyCommandResult)) {
        bytes_read += boost::asio::read(socket, boost::asio::buffer(result_buf + bytes_read, sizeof(struct VOpyCommandResult) - bytes_read));
    }
    if (result.type == BIG_READ) {
        uint32_t * data_buf = (uint32_t *) malloc(result.memory.big_read.num_of_bytes);
        bytes_read = 0;
        while (bytes_read < result.memory.big_read.num_of_bytes) {
            bytes_read += boost::asio::read(socket, boost::asio::buffer(data_buf + bytes_read, result.memory.big_read.num_of_bytes - bytes_read));
        }
        result.memory.big_read.data = data_buf;
    }
    return result;
}

