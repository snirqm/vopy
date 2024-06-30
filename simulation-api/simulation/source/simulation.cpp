// my_c_code.c
#include <svdpi.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "vopy_commands.h"
#include "server.h"


TcpSimulationServer *server = NULL;
extern "C" void server_entrypoint() {
    if (server == NULL) {
        server = new TcpSimulationServer(12345, ".");
    }
    server->start();
}


int in_fd = -1;
int out_fd = -1;
extern "C" int entrypoint()
{
    if (in_fd == -1)
    {
        printf("Opening in_fifo\n");
        in_fd = open("in_fifo", O_RDONLY);
    }
    if (in_fd < 0)
    {
        printf("Error opening in_fifo\n");
        return -1;
    }
    if (out_fd == -1)
    {
        printf("Opening out_fifo\n");
        out_fd = open("out_fifo", O_WRONLY);
    }
    if (out_fd < 0)
    {
        printf("Error opening out_fifo\n");
        return -1;
    }
    uint32_t *data_buffer = NULL;
    struct VOpyCommand cmd;
    void *cmd_buf = (void *)&cmd;
    int bytes_read = 0;
    while (bytes_read < sizeof(struct VOpyCommand))
    {
        bytes_read += read(in_fd, cmd_buf + bytes_read, sizeof(struct VOpyCommand) - bytes_read);
    }

    printf("Received command: %d\n", cmd.id);
    printf("Command type: %d\n", cmd.type);

    struct VOpyCommandResult result;
    switch (cmd.type)
    {
    case READ:
    {
        printf("Read command\n");
        printf("Address: %d\n", cmd.read.address);

        result.id = cmd.id;
        result.type = cmd.type;
        read_task(cmd.read.address);
        result.memory.data = get_result();
        result.ok = 1;
        break;
    }
    case BIG_READ:
    {
        printf("Big read command\n");
        printf("Address: %d\n", cmd.big_read.address);
        printf("Number of bytes: %d\n", cmd.big_read.num_of_bytes);
        if (cmd.big_read.num_of_bytes % 4 != 0)
        {
            printf("Number of bytes must be a multiple of 4\n");
            result.ok = 0;
            break;
        }
        data_buffer = (uint32_t *)malloc(cmd.big_read.num_of_bytes);
        bytes_read = 0;
        while (bytes_read < cmd.big_read.num_of_bytes)
        {
            read_task(cmd.big_read.address + bytes_read);
            data_buffer[bytes_read] = get_result();
            bytes_read++;
        }

        result.id = cmd.id;
        result.type = cmd.type;
        result.memory.big_read.num_of_bytes = bytes_read;
        result.ok = get_result();

        break;
    }
    case WRITE:
        printf("Write command\n");
        printf("Address: %d\n", cmd.write.address);

        result.id = cmd.id;
        result.type = cmd.type;
        write_task(cmd.write.address, cmd.write.data);
        result.ok = get_result();
        break;
    case BIG_WRITE:
    {
        printf("Big write command\n");
        printf("Address: %d\n", cmd.big_write.address);
        printf("Number of bytes: %d\n", cmd.big_write.num_of_bytes);
        if (cmd.big_write.num_of_bytes % 4 != 0)
        {
            printf("Number of bytes must be a multiple of 4\n");
            result.ok = 0;
            break;
        }
        data_buffer = (uint32_t *)malloc(cmd.big_write.num_of_bytes);
        bytes_read = 0;
        while (bytes_read < cmd.big_write.num_of_bytes)
        {
            bytes_read += read(in_fd, data_buffer + bytes_read, cmd.big_write.num_of_bytes - bytes_read);
        }
        for (int i = 0; i < cmd.big_write.num_of_bytes; i++)
        {
            write_task(cmd.big_write.address + i, data_buffer[i]);
        }
        result.id = cmd.id;
        result.type = cmd.type;
        result.ok = get_result();
        break;
    }
    case SIM_CLOCK:
        printf("Sim clock command\n");
        printf("Cycles: %d\n", cmd.sim_clock.cycles);

        sim_clock_task(cmd.sim_clock.cycles);
        
        result.id = cmd.id;
        result.type = cmd.type;
        result.ok = get_result();
        break;
    case SIM_TO_LAST:
        printf("Sim to last command\n");

        sim_to_last_task();

        result.id = cmd.id;
        result.type = cmd.type;
        result.ok = get_result();
        
        break;
    case EXIT:
        printf("Exit command\n");
        close(in_fd);
        close(out_fd);
        exit_task();
        result.ok = 1;
    case UNKNOWN:
        printf("Unknown command\n");
        break;
    }

    void *res_buf = (void *)&result;
    int bytes_written = 0;
    while (bytes_written < sizeof(struct VOpyCommandResult))
    {
        bytes_written += write(out_fd, res_buf + bytes_written, sizeof(struct VOpyCommandResult) - bytes_written);
    }

    if (cmd.type == BIG_READ)
    {
        bytes_written = 0;
        while (bytes_written < cmd.big_read.num_of_bytes)
        {
            bytes_written += write(out_fd, data_buffer + bytes_written, cmd.big_read.num_of_bytes - bytes_written);
        }
        free(data_buffer);
    }

    if (cmd.type == EXIT) {
        exit(0);
    }

    return 0;
}
