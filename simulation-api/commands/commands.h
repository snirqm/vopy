#ifndef COMMANDS_H
#define COMMANDS_H
#include <stdbool.h>
enum CommandType {
    READ,
    WRITE,
    SIM_CLOCK,
    SIM_TO_LAST,
    EXIT
} ;

struct CommandResult {
    enum CommandType type;
    union {
        struct {
            int value;
        } read;
    } result;
    bool ok;
    bool empty;
};

struct Command {
    enum CommandType type;
    union {
        struct {
            int address;
            int num_of_bytes;
        } read;

        struct {
            int address;
            char *data;
            int num_of_bytes;
        } write;

        struct {
            int cycles;
        } sim_clock;
    };
};
int execute_read(int address, int num_of_bytes, struct CommandResult *res);
int execute_write(int address, char *data, int num_of_bytes, struct CommandResult *res);
int execute_sim_clock(int cycles, struct CommandResult *res);
int execute_sim_to_last(struct CommandResult *res);
int execute_exit(struct CommandResult *res);

#endif