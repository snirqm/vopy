#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "vopy_commands.h"
#include "easylogging++.h"
#include <filesystem>
#include <optional>
using boost::asio::ip::tcp;

VOpyCommandResult TcpSimulationServer::execute(const VOpyCommand &cmd)
{
    void *cmd_buf = (void *)&cmd;
    LOG(DEBUG) << "expected " << sizeof(struct VOpyCommand) << " bytes of command to write to in_fifo (command[" << cmd.id << "])";
    int bytes_written = 0;
    while (bytes_written < sizeof(struct VOpyCommand))
    {
        bytes_written += write(fds.first, cmd_buf + bytes_written, sizeof(struct VOpyCommand) - bytes_written);
    }
    LOG(DEBUG) << "wrote " << bytes_written << " bytes of command to in_fifo (command[" << cmd.id << "])";
    if (cmd.type == BIG_WRITE)
    {
        LOG(DEBUG) << "expected " << cmd.big_write.num_of_bytes << " bytes of data to write to in_fifo (command[" << cmd.id << "])";
        bytes_written = 0;
        while (bytes_written < cmd.big_write.num_of_bytes)
        {
            bytes_written += write(fds.first, cmd.big_write.data + bytes_written, cmd.big_write.num_of_bytes - bytes_written);
        }
        LOG(DEBUG) << "wrote " << bytes_written << " bytes of data to in_fifo (command[" << cmd.id << "])";
        free(cmd.big_write.data);
    }
    LOG(INFO) << "Command[" << cmd.id << "] executed. Waiting for result...";
    struct VOpyCommandResult result;
    void *result_buf = (void *)&result;
    LOG(DEBUG) << "expected " << sizeof(struct VOpyCommandResult) << " bytes of result to read from out_fifo (command[" << cmd.id << "])";
    int bytes_read = 0;
    while (bytes_read < sizeof(struct VOpyCommandResult))
    {
        bytes_read += read(fds.second, result_buf + bytes_read, sizeof(struct VOpyCommandResult) - bytes_read);
    }
    LOG(DEBUG) << "read " << bytes_read << " bytes of result from out_fifo (command[" << cmd.id << "])";
    if (cmd.type == BIG_READ)
    {
        LOG(DEBUG) << "expected " << result.memory.big_read.num_of_bytes << " bytes of data to read from out_fifo (command[" << cmd.id << "])";
        uint32_t *data_buf = (uint32_t *) malloc(result.memory.big_read.num_of_bytes);
        bytes_read = 0;
        while (bytes_read < result.memory.big_read.num_of_bytes)
        {
            bytes_read += read(fds.second, data_buf + bytes_read, result.memory.big_read.num_of_bytes - bytes_read);
        }
        LOG(DEBUG) << "read " << bytes_read << " bytes of data from out_fifo (command[" << cmd.id << "])";
        result.memory.big_read.data = data_buf;
    }
    if (result.id != cmd.id)
    {
        LOG(ERROR) << "Command[" << cmd.id << "] result id mismatch. Expected " << cmd.id << " but got " << result.id;
        throw std::runtime_error("Command result id mismatch");
    }
    LOG(INFO) << "Result of command[" << cmd.id << "] received";
    return result;
}

std::optional<std::pair<int, int>> connect_to_simulator(const std::string &sim_path) 
{
    if (!std::filesystem::exists(sim_path)) {
        LOG(ERROR) << "Simulation path does not exist";
        return std::nullopt;
    }
    if (!std::filesystem::is_directory(sim_path)) {
        LOG(ERROR) << "Simulation path is not a directory";
        return std::nullopt;
    }
    std::string in_fifo_path = sim_path + "/in_fifo";
    std::string out_fifo_path = sim_path + "/out_fifo";
    if (std::filesystem::exists(in_fifo_path)) {
        LOG(WARNING) << "in_fifo already exists.";
    } else if (mkfifo(in_fifo_path.c_str(), 0777) < 0) {
        LOG(ERROR) << "Error creating in_fifo";
        return std::nullopt;
    }
    if (std::filesystem::exists(out_fifo_path)) {
        LOG(WARNING) << "out_fifo already exists.";
        std::filesystem::remove(out_fifo_path);
    } else if (mkfifo(out_fifo_path.c_str(), 0777) < 0) {
        LOG(ERROR) << "Error creating out_fifo";
        return std::nullopt;
    }
    int in_fd = open(in_fifo_path.c_str(), O_WRONLY);
    if (in_fd < 0) {
        LOG(ERROR) << "Error opening in_fifo";
        return std::nullopt;
    }
    int out_fd = open(out_fifo_path.c_str(), O_RDONLY);
    if (out_fd < 0) {
        LOG(ERROR) << "Error opening out_fifo";
        close(in_fd);
        return std::nullopt;
    }
    return std::make_pair(in_fd, out_fd);
}

void TcpSimulationServer::start()
{
    LOG(INFO) << "Starting server";
    LOG(INFO) << "IP: " << acceptor.local_endpoint().address().to_string();
    LOG(INFO) << "PORT: " << acceptor.local_endpoint().port();
    LOG(INFO) << "SIM_PATH: " << sim_path;
    auto result = connect_to_simulator(sim_path);
    if (!result.has_value()) {
        LOG(ERROR) << "Failed to connect to simulator";
        return;
    }
    fds = result.value();
    acceptor.accept(socket);
    while (true)
    {
        try
        {
            std::cout << "Waiting for command..." << std::endl;
            auto command = receive_command();
            std::cout << "Command " << cmd_to_string(command) << " recived" << std::endl;
            send_result(execute(command));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

TcpSimulationServer::TcpSimulationServer(int port,
                                         const std::string &sim_path) 
                                         : acceptor(io_context, tcp::endpoint(tcp::v4(), port)), 
                                         socket(io_context), 
                                         sim_path(std::move(sim_path)) {}

VOpyCommand TcpSimulationServer::receive_command()
{
    VOpyCommand cmd;
    void *cmd_buf = (void *)&cmd;
    run_or_accept<void>([&]()
                        {
        int bytes_read = 0;
        while (bytes_read < sizeof(struct VOpyCommand)) {
            bytes_read += boost::asio::read(socket, boost::asio::buffer(cmd_buf + bytes_read, sizeof(struct VOpyCommand) - bytes_read));
        } });
    if (cmd.type == BIG_WRITE)
    {
        uint32_t *data_buf = (uint32_t *) malloc(cmd.big_write.num_of_bytes);
        run_or_accept<void>([&]()
                            {
            int bytes_read = 0;
            while (bytes_read < cmd.big_write.num_of_bytes) {
                bytes_read += boost::asio::read(socket, boost::asio::buffer(data_buf + bytes_read, cmd.big_write.num_of_bytes - bytes_read));
            } });
        cmd.big_write.data = data_buf;
    }
    return cmd;
}

void TcpSimulationServer::send_result(const VOpyCommandResult &res)
{
    std::cout << "Sending result " << result_to_string(res) << std::endl;
    void *res_buf = (void *)&res;
    run_or_accept<void>([&]()
                        {
        int bytes_written = 0;
        while (bytes_written < sizeof(struct VOpyCommandResult)) {
            bytes_written += boost::asio::write(socket, boost::asio::buffer(res_buf + bytes_written, sizeof(struct VOpyCommandResult) - bytes_written));
        } });
    if (res.type == BIG_READ)
    {
        run_or_accept<void>([&]()
                            {
            int bytes_written = 0;
            while (bytes_written < res.memory.big_read.num_of_bytes) {
                bytes_written += boost::asio::write(socket, boost::asio::buffer(res.memory.big_read.data + bytes_written, res.memory.big_read.num_of_bytes - bytes_written));
            } });
        free(res.memory.big_read.data);
    }
}
