#pragma once
#include <svdpi.h>
#include "vopy_commands.h"

extern "C" {
  // imported functions from SystemVerilog to C
  void read_task(uint64_t address, uint16_t num_of_bytes, svOpenArrayHandle data);
  void write_task(uint64_t address, uint16_t num_of_bytes, const svOpenArrayHandle data);
  void drive_clock_task(uint64_t cycles, uint64_t *clk_count);
  void exit_task();

  // exported function from C to SystemVerilog
  int start_server(int port_number);
  int event_loop();
}