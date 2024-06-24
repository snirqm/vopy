#include "commands.h"

int execute_read(int address, int num_of_bytes, struct CommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual read operation
    res->type = READ;
    res->result.read.value = address + 42;
    res->ok = true;
    return 0;
}

int execute_write(int address, char *data, int num_of_bytes, struct CommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual write operation
    res->type = WRITE;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_sim_clock(int cycles, struct CommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual simulation clock operation
    res->type = SIM_CLOCK;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_sim_to_last(struct CommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual simulation to last operation
    res->type = SIM_TO_LAST;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_exit(struct CommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual exit operation
    res->type = EXIT;
    res->ok = true;
    res->empty = true;
    return 0;
}