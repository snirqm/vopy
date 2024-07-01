#include "simulation.h"
#include "server.h"
#define BUFFER_SIZE 1024

TcpSimulationServer *server = NULL;

char buffer[BUFFER_SIZE];
int start_server(int port)
{
    if (server != NULL) {
        return -1;
    }
    server = new TcpSimulationServer(port, ".");
    server->start();
    return 0;
}
int event_loop()
{
    auto command = server->receive_command(buffer);
    struct VOpyCommandResult result {
        .id = command.id,
        .type = command.type,
        .ok = 1
    };
    switch (command.type)
    {
        case READ:
        {
            if (command.read.num_of_bytes > BUFFER_SIZE) {
                result.ok = 0;
                break;
            }
            read_task(command.read.address, command.read.num_of_bytes, buffer);
            result.data_size = command.read.num_of_bytes;
            break;
        }
        case WRITE:
        {
            write_task(command.write.address, command.write.num_of_bytes, buffer);
            break;
        }
        case SIM_CLOCK:
        {
            uint64_t clk_count = 0;
            drive_clock_task(command.sim_clock.cycles, &clk_count);
            break;
        }
        case SIM_TO_LAST:
        {
            exit_task();
            break;
        }
    }
    server->send_result(result, buffer);
    return 0;
}
