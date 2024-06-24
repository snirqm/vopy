#include "server.hpp"
#include <string>
#include <sys/file.h>
#include <print>
#include <sys/stat.h>


//
// FileSimulationServer
//

FileSimulationServer::FileSimulationServer(const std::string& in_filename, const std::string& out_filename)
        : in_filename(in_filename), out_filename(out_filename) {
    if (access(in_filename.c_str(), F_OK) == -1) {
        std::cerr << "Input file does not exist" << std::endl;
        mkfifo(in_filename.c_str(), 0777);
    }
    if (access(out_filename.c_str(), F_OK) == -1) {
        std::cerr << "Output file does not exist" << std::endl;
        mkfifo(out_filename.c_str(), 0777);
    }
}


struct CommandResult VOpySimulationServer::execute(const Command &cmd) {
    struct CommandResult result{};
    switch (cmd.type) {
        case READ:
            execute_read(cmd.read.address, cmd.read.num_of_bytes, &result);
            break;
        case WRITE:
            execute_write(cmd.write.address, cmd.write.data, cmd.write.num_of_bytes, &result);
            break;
        case SIM_CLOCK:
            execute_sim_clock(cmd.sim_clock.cycles, &result);
            break;
        case SIM_TO_LAST:
            execute_sim_to_last(&result);
            break;
        case EXIT:
            execute_exit(&result);
            exit(0);
    }
    return result;
}

void VOpySimulationServer::start() {
    while (true) {
        try {
            auto response = execute(receive_command());
            if (!response.empty) send_result(response);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

Command FileSimulationServer::receive_command() {
    int fd_in = open(in_filename.c_str(), O_RDONLY);
    if (fd_in == -1) {
        throw std::runtime_error("Failed to open input file");
    }
    char buffer[1024];
    ssize_t bytes_read = read(fd_in, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        throw std::runtime_error("Failed to read from input file");
    }

    if (bytes_read == 0) {
        throw std::runtime_error("Failed to read from input file: no data");
    }
    close(fd_in);

    std::string message(buffer, bytes_read);
    return from_string(message);
}



void FileSimulationServer::send_result(const struct CommandResult &res) {
    auto response = to_string(res);
    std::cout << "Sending response: " << response << std::endl;
    int fd_out = open(out_filename.c_str(), O_WRONLY);
    if (fd_out == -1) {
        throw std::runtime_error("Failed to open output file");
    }

    ssize_t bytes_written = write(fd_out, response.c_str(), response.size());
    close(fd_out);
    if (bytes_written == -1) {
        throw std::runtime_error("Failed to write to output file");
    }
}
