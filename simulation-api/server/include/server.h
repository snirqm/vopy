#pragma once
#include "vopy_commands.h"
#include "easylogging++.h"

#include <string>
#include <boost/asio.hpp>
#include <iostream>


class TcpSimulationServer  {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    VOpyCommand receive_command();
    void send_result(const VOpyCommandResult &res);
    static VOpyCommandResult execute(const VOpyCommand &cmd);

    template<typename T>
    T run_or_accept(std::function<T()> f) {
        try {
            return f();
        } catch (const std::exception &e) {
            LOG(ERROR) << "Error: " << e.what();
            socket.close();
            acceptor.accept(socket);
            return f();
        }
    }
public:
    explicit TcpSimulationServer(int port);
    void start();
};
