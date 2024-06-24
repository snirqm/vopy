#pragma once
#include "commands.h"
#include <string>
#include <boost/asio.hpp>
#include <iostream>
class VOpySimulationServer {
    virtual struct Command receive_command() = 0;
    virtual void send_result(const struct CommandResult &res) = 0;

    virtual struct CommandResult execute(const struct Command &cmd);
public:
    virtual ~VOpySimulationServer() = default;
    void start();
};

class TcpSimulationServer : public VOpySimulationServer {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    struct Command receive_command() override;
    void send_result(const struct CommandResult &res) override;

    template<typename T>
    T run_or_accept(std::function<T()> f) {
        try {
            return f();
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            socket.close();
            acceptor.accept(socket);
            return f();
        }
    }
public:
    explicit TcpSimulationServer(int port);
};

class FileSimulationServer : public VOpySimulationServer {
    const std::string in_filename;
    const std::string out_filename;
    struct Command receive_command() override;
    void send_result(const struct CommandResult &res) override;
public:
    FileSimulationServer(const std::string& in_filename, const std::string& out_filename);
};


//
// Utils
//

static std::string to_string(struct CommandResult result) {
    switch (result.type) {
        case READ:
            return std::to_string(result.result.read.value);
        case SIM_CLOCK:
        case SIM_TO_LAST:
        case WRITE:
        case EXIT:
            throw std::runtime_error("Invalid result type");
    }
}

static Command from_string(const std::string &command) {
    Command cmd{};
    std::cout << "Received command: " << command;
    if (command.find("exit") == 0) { // for example: exit
        cmd.type = EXIT;
    } else if (command.find("read") == 0) { // for example: read 0
        cmd.type = READ;
        cmd.read.address = std::stoi(command.substr(5));
    } else if (command.find("write") == 0) { // for example: write 0
        cmd.type = WRITE;
        cmd.write.address = std::stoi(command.substr(6));


    }
    return cmd;
}