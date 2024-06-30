#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>
#include <stdlib.h>

struct VOpySimulationConfig {
    char *sim_in_fifo_path;
    char *sim_out_fifo_path;
};

enum VOpyCommandType {
    READ,
    BIG_READ,
    WRITE,
    BIG_WRITE,
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
            uint16_t num_of_bytes;    
            uint32_t *data;
        } big_read;
        uint32_t data;
    } memory;
    bool ok;
};

struct VOpyCommand {
    int id;
    enum VOpyCommandType type;
    union {
        struct {
            uint64_t address;
        } read;

        struct {
            uint64_t address;
            uint32_t data;
        } write;

        struct {
            uint64_t address;
            uint16_t num_of_bytes;
        } big_read;

        struct {
            uint64_t address;
            uint16_t num_of_bytes;
            uint32_t *data;
        } big_write;

        struct {
            uint64_t cycles;
        } sim_clock;
    };
};
#ifdef __cplusplus
extern "C" {
#endif

void read_task(uint64_t address);
void write_task(uint64_t address, uint32_t data);
void sim_clock_task(uint64_t cycles);
void sim_to_last_task();
void exit_task();
int get_result();


#ifdef __cplusplus
}
#endif

inline enum VOpyCommandType type_from_string(const char* type) {
    if (strcmp(type, "big_read") == 0) {
        return BIG_READ;
    } else if (strcmp(type, "read") == 0) {
        return READ;
    } else if (strcmp(type, "big_write") == 0) {
        return BIG_WRITE;
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

#ifdef __cplusplus
#include <boost/format.hpp>
inline std::string cmd_to_string(struct VOpyCommand cmd) {
    switch (cmd.type) {
        case READ: {// cmd.read.address is uint64_t
            return (boost::format("%1%:read %2%") % cmd.id % cmd.read.address).str(); 
        }
        case BIG_READ: {
            return (boost::format("%1%:big_read %2% %3%") % cmd.id % cmd.big_read.address % cmd.big_read.num_of_bytes).str();
        }
        case WRITE: {
            return (boost::format("%1%:write %2% %3%") % cmd.id % cmd.write.address % cmd.write.data).str();
        }
        case BIG_WRITE: {
            return (boost::format("%1%:big_write %2% %3%") % cmd.id % cmd.big_write.address % cmd.big_write.num_of_bytes).str();
        }
        case SIM_CLOCK: {
            return (boost::format("%1%:sim_clock %2%") % cmd.id % cmd.sim_clock.cycles).str();
        }
        case SIM_TO_LAST: {
            return (boost::format("%1%:sim_to_last") % cmd.id).str();
        }
        case EXIT: {
            return (boost::format("%1%:exit") % cmd.id).str();
        }
        case UNKNOWN:
            return NULL;
    }
    return NULL;
}

inline std::string result_to_string(VOpyCommandResult res) {
    switch (res.type) {
        case READ: {
            return (boost::format("%d:read %d") % res.id % res.memory.data).str();
        }
        case WRITE: {
            return (boost::format("%d:write %d") % res.id % res.memory.data).str();
        }
        case BIG_READ: {
            return (boost::format("%d:big_read %d") % res.id % res.memory.big_read.num_of_bytes).str();
        }
        case BIG_WRITE: {
            return (boost::format("%d:big_write %d") % res.id % res.memory.big_read.num_of_bytes).str();
        }
        case SIM_CLOCK: {
            return (boost::format("%d:sim_clock %d") % res.id % res.memory.data).str();
        }
        case SIM_TO_LAST: {
            return (boost::format("%d:sim_to_last") % res.id).str();
        }
        case EXIT: {
            return (boost::format("%d:exit") % res.id).str();
        }
        case UNKNOWN:
            return NULL;
    }
    return NULL;
}

#endif



#endif