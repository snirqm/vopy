#pragma once

#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include "vopy_commands.h"


class TcpSimulationServer  {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    std::string sim_path;

    template<typename T>
    T run_or_accept(std::function<T()> f) {
        try {
            return f();
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << std::endl;
            socket.close();
            acceptor.accept(socket);
            return f();
        }
    }
    void send_data(const void *data, uint32_t num_of_bytes);
    void receive_data(void *data, uint32_t num_of_bytes);
public:
    VOpyCommand receive_command(char *buffer);
    void send_result(const VOpyCommandResult &res, char *buffer);
    TcpSimulationServer(int port, const std::string &sim_path);
    void start();
};
