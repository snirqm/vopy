#include "app.h"
#include "easylogging++.h"
#include <memory>

#define BUFFER_SIZE 1024
#define PORT_FLAG "-p"
#define HOST_FLAG "-i"

namespace po = boost::program_options;

char buffer[BUFFER_SIZE];

VOpyClientConfig::VOpyClientConfig(int argc, char **argv)
{
    argv = app.ensure_utf8(argv);
    app.add_option(HOST_FLAG, host, "Server host");
    app.add_option(PORT_FLAG, port, "Server port");
    parse(argc, argv);
}

int VOpyClientConfig::parse(int argc, char **argv)
{
    CLI11_PARSE(app, argc, argv)
    std::cout << "Host: " << host << std::endl;
    std::cout << "Port: " << port << std::endl;
    endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port);
    return 0;
}

VOpyClientApp::VOpyClientApp(int argc, char **argv) 
{
    config = std::make_unique<VOpyClientConfig>(argc, argv);
    client = std::make_unique<VOpyTcpClient>(config->endpoint);
}

void print_help()
{
    std::cout << "List of commands:" << std::endl;
    std::cout << "  read <address> <num_of_bytes>" << std::endl;
    std::cout << "  write <address> <num_of_bytes> <bytes[0]> <bytes[1]> ... <bytes[num_of_bytes - 1]>" << std::endl;
    std::cout << "  sim_clock <cycles>" << std::endl;
    std::cout << "  sim_to_last" << std::endl;
    std::cout << "  exit" << std::endl;
    std::cout << std::flush;
}

uint64_t parse_address(const std::string address)
{
    return std::stoull(address, nullptr, 0);
}

uint32_t parse_data(const std::string data)
{
    return std::stoul(data, nullptr, 0);
}

uint8_t parse_byte(const std::string byte)
{
    return std::stoul(byte, nullptr, 16);
}

void process_result(VOpyCommand command, VOpyCommandResult result, char *buffer)
{
    if (!result.ok)
    {
        std::cout << "Command failed" << std::endl;
        return;
    }
    if (result.type == READ)
    {
        std::stringstream streambuf;a
        for (size_t i = 0; i < result.data_size ; i++)
        {
            if (i % 4 == 0) streambuf << "0x" << std::hex << command.read.address + i << ": ";
            boost::format fmt("%02x ");
            auto chunk = ((unsigned int)buffer[i] & 0xff);
            streambuf << fmt % chunk;
            if (i % 4 == 3 && i != result.data_size - 1) streambuf << std::endl;
        }
        std::cout << streambuf.str() << std::endl;
    }
}

inline enum VOpyCommandType type_from_string(const char* type) {
    if (strcmp(type, "read") == 0) {
        return READ;
    } else if (strcmp(type, "write") == 0) {
        return WRITE;
    } else if (strcmp(type, "sim_clock") == 0) {
        return SIM_CLOCK;
    } else if (strcmp(type, "sim_to_last") == 0) {
        return SIM_TO_LAST;
    } else if (strcmp(type, "exit") == 0) {
        return EXIT;
    } else {
        LOG(ERROR) << boost::format("Unknown command type: %1%") % type;
        return UNKNOWN;
    }
}

VOpyCommand VOpyClientApp::read_command_from_stdin()
{
    std::cout << " qoco>";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<std::string> args(std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>());
    if (args.empty())
    {
        return read_command_from_stdin();
    }
    if (args[0] == "help")
    {
        print_help();
        return read_command_from_stdin();
    }
    VOpyCommand cmd{.type = type_from_string(args[0].c_str())};

    switch (cmd.type)
    {
    case READ:
    {
        if (args.size() != 3)
        {
            std::cout << "Invalid number of arguments" << std::endl;
            return read_command_from_stdin();
        }
        auto address = parse_address(args[1]);
        auto num_bytes = parse_data(args[2]);

        cmd.type = READ;
        cmd.read.address = address;
        cmd.read.num_of_bytes = num_bytes;
        break;
    }
    case WRITE:
    {
        if (args.size() < 3)
        {
            std::cout << " ERROR: Invalid number of arguments" << std::endl;
            return read_command_from_stdin();
        }
        auto address = parse_address(args[1]);
        auto num_bytes = parse_data(args[2]);
        std::vector<uint8_t> data;
        for (size_t i = 3; i < args.size(); i++)
        {
            data.push_back(parse_byte(args[i]));
        }
        while (data.size() < num_bytes)
        {
            std::cout << " qoco>" << std::flush;
            std::getline(std::cin, line);
            std::istringstream iss(line);
            std::vector<std::string> args(std::istream_iterator<std::string>{iss},
                                          std::istream_iterator<std::string>());
            for (size_t i = 0; i < args.size(); i++)
            {
                data.push_back(parse_byte(args[i]));
            }
        }

        cmd.type = WRITE;
        cmd.write.address = address;
        cmd.write.num_of_bytes = num_bytes;
        for (size_t i = 0; i < data.size(); i++)
        {
            buffer[i] = data[i];
        }
        break;
    }
    default:
        print_help();
        return read_command_from_stdin();
    }
    return cmd;
}

int VOpyClientApp::run()
{
    client->connect();
    while (true)
    {
        auto cmd = read_command_from_stdin();
        client->send_command(cmd, buffer);
        auto result = client->receive_result(buffer);
        process_result(cmd, result, buffer);
    }
    return 0;
}
