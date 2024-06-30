#include "app.h"
#include "easylogging++.h"
#include <memory>

#define PORT_FLAG "-p"
#define HOST_FLAG "-i"

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

void help()
{
    std::cout << "Commands: " << std::endl;
    std::cout << "read <address>" << std::endl;
    std::cout << "write <address> <value>" << std::endl;
    std::cout << "big_read <address> <num_of_bytes>" << std::endl;
    std::cout << "big_write <address> <num_of_bytes> <bytes[0]> <bytes[1]> ... <bytes[num_of_bytes - 1]>" << std::endl;
    std::cout << "sim_clock <cycles>" << std::endl;
    std::cout << "sim_to_last" << std::endl;
    std::cout << "exit" << std::endl;
}

uint64_t parse_address(const std::string address)
{
    return std::stoull(address, nullptr, 0);
}

uint32_t parse_data(const std::string data)
{
    return std::stoul(data, nullptr, 0);
}

static VOpyCommand read_command_from_stdin()
{
    // get a line from stdin
    std::string line;
    std::getline(std::cin, line);
    if (line == "help")
    {
        help();
        return read_command_from_stdin();
    }
    std::istringstream iss(line);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>());
    // create a command
    VOpyCommand cmd{.type = UNKNOWN};
    if (tokens.size() == 0)
    {
        return cmd;
    }
    switch (type_from_string(tokens[0].c_str()))
    {
    case READ:
    {
        if (tokens.size() != 2)
        {
            help();
            break;
        }
        cmd.type = READ;
        cmd.read.address = parse_address(tokens[1]);
        break;
    }
    case BIG_READ:
    {
        if (tokens.size() != 3)
        {
            help();
            break;
        }
        cmd.type = BIG_READ;
        cmd.big_read.address = parse_address(tokens[1]);
        cmd.big_read.num_of_bytes = std::stoul(tokens[2]);
        break;
    }
    case WRITE:
    {
        if (tokens.size() != 3)
        {
            help();
            break;
        }
        cmd.type = WRITE;
        cmd.write.address = parse_address(tokens[1]);
        cmd.write.data = parse_data(tokens[2]);
        break;
    }
    case BIG_WRITE:
    {
        if (tokens.size() != 3)
        {
            help();
            break;
        }
        cmd.type = BIG_WRITE;
        cmd.big_write.address = parse_address(tokens[1]);
        cmd.big_write.num_of_bytes = std::stoul(tokens[2]);
        auto data_tokens = std::vector<uint32_t>();
        for (int i = 0; i < tokens.size() - 3; i++)
        {
            data_tokens.push_back(parse_data(tokens[3 + i]));
        }
        while (data_tokens.size() < cmd.big_write.num_of_bytes * 4)
        {
            std::getline(std::cin, line);
            std::istringstream iss(line);
            for (int i = 0; i < 4; i++)
            {
                std::string token;
                iss >> token;
                data_tokens.push_back(parse_data(token));
            }
        }
        cmd.big_write.data = (uint32_t *)malloc(cmd.big_write.num_of_bytes);
        for (int i = 0; i < cmd.big_write.num_of_bytes; i++)
        {
            cmd.big_write.data[i] = data_tokens[i];
        }
        break;
    }
    case SIM_CLOCK:
    {
        if (tokens.size() != 2)
        {
            help();
            break;
        }
        cmd.type = SIM_CLOCK;
        cmd.sim_clock.cycles = std::stoull(tokens[1]);
        break;
    }
    case SIM_TO_LAST:
    {
        if (tokens.size() != 1)
        {
            help();
            break;
        }
        cmd.type = SIM_TO_LAST;
        break;
    }
    case EXIT:
    {
        if (tokens.size() != 1)
        {
            help();
            break;
        }
        cmd.type = EXIT;
        break;
    }
    }
    return cmd;
}

int VOpyClientApp::run()
{
    int count = 0;
    client->connect();
    while (true)
    {
        auto cmd = read_command_from_stdin();
        cmd.id = count++;
        client->send_command(cmd);
        auto result = client->receive_result();
        if (!result.ok)
        {
            LOG(WARNING) << "!!!NOT OK result!!!";
        }
        switch (result.type)
        {
        case READ:
        {
            std::cout << "Read result: " << result.memory.data << std::endl;
            break;
        }
        case WRITE:
        {
            std::cout << "Write result: " << result.memory.data << std::endl;
            break;
        }
        case BIG_READ:
        {
            std::cout << "Big read result: " << std::endl;
            for (int i = 0; i < result.memory.big_read.num_of_bytes; i++)
            {
                std::cout << cmd.big_read.address + i << ": " << ((char *)result.memory.big_read.data)[i] << std::endl;
            }
            free(result.memory.big_read.data);
            break;
        }
        case BIG_WRITE:
        {
            std::cout << "Big write result: " << result.ok << std::endl;
            free(cmd.big_write.data);
            break;
        }
        case SIM_CLOCK:
        {
            std::cout << "Sim clock result: " << result.memory.data << std::endl;
            break;
        }
        case SIM_TO_LAST:
        {
            std::cout << "Sim to last result" << std::endl;
            break;
        }
        case EXIT:
        {
            std::cout << "Exit result" << std::endl;
            break;
        }
        case UNKNOWN:
        {
            std::cout << "Unknown result" << std::endl;
            break;
        }
        }
    }
    return 0;
}

