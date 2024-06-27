#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

enum VOpyCommandType {
    READ,
    WRITE,
    SIM_CLOCK,
    SIM_TO_LAST,
    EXIT,
    UNKNOWN
};

struct VOpyCommandResult {
    int id;
    enum VOpyCommandType type;
    union {
        struct {
            int value;
        } read;
    } result;
    bool ok;
    bool empty;
};

struct VOpyCommand {
    int id;
    enum VOpyCommandType type;
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
#ifdef __cplusplus
extern "C" {
#endif

int execute_read(int address, int num_of_bytes, struct VOpyCommandResult *res);

int execute_write(int address, char *data, int num_of_bytes, struct VOpyCommandResult *res);

int execute_sim_clock(int cycles, struct VOpyCommandResult *res);

int execute_sim_to_last(struct VOpyCommandResult *res);

int execute_exit(struct VOpyCommandResult *res);

char *result_to_string(struct VOpyCommandResult result);
char *cmd_to_string(struct VOpyCommand cmd);
struct VOpyCommand cmd_from_string(const char *command);
struct VOpyCommandResult result_from_string(const char *command_result);


#ifdef __cplusplus
}
#endif
#endif