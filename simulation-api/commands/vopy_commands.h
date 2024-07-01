#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <boost/format.hpp>
struct VOpySimulationConfig {
    char *sim_in_fifo_path;
    char *sim_out_fifo_path;
};

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
    bool ok;
    uint32_t data_size;
};

struct VOpyCommand {
    int id;
    enum VOpyCommandType type;
    union {
        struct {
            uint64_t address;
            uint32_t num_of_bytes;
        } read;

        struct {
            uint64_t address;
            uint32_t num_of_bytes;
        } write;

        struct {
            uint64_t cycles;
        } sim_clock;
    };
};


