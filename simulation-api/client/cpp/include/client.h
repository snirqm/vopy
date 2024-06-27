#pragma once
#include <string>
#include <boost/asio.hpp>
#include "vopy_commands.h"

class VOpyTcpClient  {
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket;
public:
    VOpyTcpClient(boost::asio::ip::tcp::endpoint endpoint);
    void connect();
    void disconnect();
    void send_command(const VOpyCommand &cmd);
    VOpyCommandResult receive_result();
};
