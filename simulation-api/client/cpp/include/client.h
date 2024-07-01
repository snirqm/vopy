#pragma once
#include <string>
#include <boost/asio.hpp>
#include "vopy_commands.h"

class VOpyTcpClient  {
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket;
    void send_data(const void *data, uint32_t num_of_bytes);
    void receive_data(void *data, uint32_t num_of_bytes);
public:
    VOpyTcpClient(boost::asio::ip::tcp::endpoint endpoint);
    void connect();
    void disconnect();
    void send_command(const VOpyCommand &cmd, char *buffer);
    VOpyCommandResult receive_result(char *buffer);
};
