#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vopy_commands.h"

int execute_read(int address, int num_of_bytes, struct VOpyCommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual read operation
    res->type = READ;
    res->result.read.value = address + 42;
    res->ok = true;
    return 0;
}

int execute_write(int address, char *data, int num_of_bytes, struct VOpyCommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual write operation
    res->type = WRITE;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_sim_clock(int cycles, struct VOpyCommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual simulation clock operation
    res->type = SIM_CLOCK;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_sim_to_last(struct VOpyCommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual simulation to last operation
    res->type = SIM_TO_LAST;
    res->ok = true;
    res->empty = true;
    return 0;
}

int execute_exit(struct VOpyCommandResult *res) {
    // This is a dummy implementation
    // TODO: Implement the actual exit operation
    res->type = EXIT;
    res->ok = true;
    res->empty = true;
    return 0;
}

char *cmd_to_string(struct VOpyCommand cmd) {
    switch (cmd.type) {
        case READ: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:read %d %d", cmd.id, cmd.read.address, cmd.read.num_of_bytes);
            return str;
        }
        case WRITE: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:write %d %d %s", cmd.id, cmd.write.address, cmd.write.num_of_bytes, cmd.write.data);
            return str;
        }

        case SIM_CLOCK: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:sim_clock %d", cmd.id, cmd.sim_clock.cycles);
            return str;
        }
        case SIM_TO_LAST: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:sim_to_last", cmd.id);
            return str;
        }
        case EXIT: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:exit", cmd.id);
            return str;
        }
        case UNKNOWN:
            return NULL;
    }

}

char *result_to_string(struct VOpyCommandResult result) {
    switch (result.type) {
        case READ: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:read %d", result.id, result.result.read.value);
            return str;
        }
        case WRITE:{
            char *str = (char *) malloc(100);
            sprintf(str, "%d:write ok", result.id);
            return str;
        }
        case SIM_CLOCK: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:sim_clock ok", result.id);
            return str;
        }
        case SIM_TO_LAST:{
            char *str = (char *) malloc(100);
            sprintf(str, "%d:sim_to_last ok", result.id);
            return str;
        }
        case EXIT: {
            char *str = (char *) malloc(100);
            sprintf(str, "%d:exit ok", result.id);
            return str;
        }
        case UNKNOWN:
            return NULL;
    }

}

static enum VOpyCommandType type_from_string(const char* type) {
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
        return UNKNOWN;
    }
}

struct VOpyCommand cmd_from_string(const char *command) {
    int id;
    char type[20];
    struct VOpyCommand cmd;
    // get the id and type of the command
    sscanf(command, "%d:%s", &id, type);
    cmd.id = id;
    cmd.type = type_from_string(type);
    switch (cmd.type) {
        case READ:
            sscanf(command, "%d:read %d %d", &id, &cmd.read.address, &cmd.read.num_of_bytes);
            break;
        case WRITE:
            sscanf(command, "%d:write %d %d", &id, &cmd.write.address, &cmd.write.num_of_bytes);
            break;
        case SIM_CLOCK:
            sscanf(command, "%d:sim_clock %d", &id, &cmd.sim_clock.cycles);
            break;
        case SIM_TO_LAST:
        case EXIT:
            break;
        case UNKNOWN:
            break;
    }
    return cmd;
}

struct VOpyCommandResult result_from_string(const char *command_result) {
    int id;
    char type[20];
    struct VOpyCommandResult result;
    // get the id and type of the command
    sscanf(command_result, "%d:%s", &id, type);
    result.id = id;
    result.type = type_from_string(type);
    switch (result.type) {
        case READ:
            sscanf(command_result, "%d:read %d", &id, &result.result.read.value);
            result.ok = true;
            break;
        case WRITE:
        case SIM_CLOCK:
        case SIM_TO_LAST:
        case EXIT:
            result.ok = true;
            break;
        case UNKNOWN:
            break;
    }
    return result;

}

