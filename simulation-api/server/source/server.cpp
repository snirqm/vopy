#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "vopy_commands.h"
#include "easylogging++.h"
#include <filesystem>
#include <optional>
using boost::asio::ip::tcp;

void TcpSimulationServer::start()
{
    LOG(INFO) << "Starting server";
    LOG(INFO) << "IP: " << acceptor.local_endpoint().address().to_string();
    LOG(INFO) << "PORT: " << acceptor.local_endpoint().port();
    LOG(INFO) << "SIM_PATH: " << sim_path;
    acceptor.accept(socket);
}

TcpSimulationServer::TcpSimulationServer(int port,
                                         const std::string &sim_path) 
                                         : acceptor(io_context, tcp::endpoint(tcp::v4(), port)), 
                                         socket(io_context), 
                                         sim_path(std::move(sim_path)) {}

VOpyCommand TcpSimulationServer::receive_command(char *buffer)
{
    VOpyCommand cmd;
    void *cmd_buf = (void *)&cmd;
    run_or_accept<void>([&]()
                        {
        int bytes_read = 0;
        while (bytes_read < sizeof(struct VOpyCommand)) {
            bytes_read += boost::asio::read(socket, boost::asio::buffer(cmd_buf + bytes_read, sizeof(struct VOpyCommand) - bytes_read));
        } 
    });
    if (cmd.type == WRITE)
    {
        receive_data(buffer, cmd.write.num_of_bytes);
    }     
    return cmd;
}

void TcpSimulationServer::send_result(const VOpyCommandResult &res, char *buffer)
{
    void *res_buf = (void *)&res;
    run_or_accept<void>([&]()
                        {
        int bytes_written = 0;
        while (bytes_written < sizeof(struct VOpyCommandResult)) {
            bytes_written += boost::asio::write(socket, boost::asio::buffer(res_buf + bytes_written, sizeof(struct VOpyCommandResult) - bytes_written));
        } });
    if (res.type == READ) 
    {
        send_data(buffer, res.data_size);
    }
}


void TcpSimulationServer::send_data(const void *data, uint32_t num_of_bytes)
{
    run_or_accept<void>([&]()
                        {
        int bytes_written = 0;
        while (bytes_written < num_of_bytes) {
            bytes_written += boost::asio::write(socket, boost::asio::buffer(data + bytes_written, num_of_bytes - bytes_written));
        } 
        });
}

void TcpSimulationServer::receive_data(void *data, uint32_t num_of_bytes)
{
    run_or_accept<void>([&]()
                        {
        int bytes_read = 0;
        while (bytes_read < num_of_bytes) {
            bytes_read += boost::asio::read(socket, boost::asio::buffer(data + bytes_read, num_of_bytes - bytes_read));
        } });
}