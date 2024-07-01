module my_test;
  `define MEMORY_SIZE 1024
  `define REG_SIZE 32
  `define MAX_BYTES 256  // Define a maximum number of bytes for read/write operations

  import "DPI-C" context task start_server(input int port_number);
  import "DPI-C" context task event_loop();

  // Declare and export the functions and tasks
  export "DPI-C" task read_task;
  export "DPI-C" task write_task;
  export "DPI-C" task drive_clock_task;
  export "DPI-C" task exit_task;

  logic clk;
  int unsigned clk_counter;
  int unsigned port_number;
  bit finish_test = 0;
  int index;
  logic [7:0] memory [0:`MEMORY_SIZE - 1];

  initial begin
    for (int i = 0; i < `MEMORY_SIZE; i++) begin
      memory[i] = 0;
    end
  end

  task read_task(input longint unsigned address, input shortint unsigned num_bytes, output byte unsigned data[`MAX_BYTES]);
    if (address + num_bytes > `MEMORY_SIZE) begin
      $display("Error: Read operation out of bounds. Address: %0d, Bytes: %0d", address, num_bytes);
      return;
    end
    repeat (10) @(posedge clk);
    for (index = 0; index < num_bytes; index++) begin
      data[index] = memory[address + index];
      repeat (1) @(posedge clk);
    end
  endtask

  task write_task(input longint unsigned address, input shortint unsigned num_bytes, input byte unsigned data[`MAX_BYTES]);
    if (address + num_bytes > `MEMORY_SIZE) begin
      $display("Error: Write operation out of bounds. Address: %0d, Bytes: %0d", address, num_bytes);
      return;
    end
    repeat (10) @(posedge clk);
    for (index = 0; index < num_bytes; index++) begin
      memory[address + index] = data[index];
      repeat (1) @(posedge clk);
    end
  endtask

  task drive_clock_task(input longint unsigned cycles, output longint unsigned clk_count);
  if (cycles == 0) begin
    // If the number of cycles is 0, then drive the simulation until the finish_test flag is set
    while (finish_test == 0) begin
      @(posedge clk);
    end
    $display("Simulation finished");
    $finish;
  end else begin
    // Otherwise, drive the simulation for the specified number of cycles
    repeat (cycles) @(posedge clk);
  end
    // Return the number of clock cycles that have been driven
    clk_count = clk_counter;
  endtask

  task exit_task();
    finish_test = 1;
  endtask
  

  initial begin
    int port_number = 8080;
    if (!$value$plusargs("PORT=%d", port_number)) $display("Using default port number: %0d", port_number);
    start_server(port_number);
    while (finish_test == 0) begin
      event_loop();
    end
    $finish;
  end

  initial begin
    clk = 0;
    clk_counter = 0;
  end

  always #5 clk <= ~clk;

  always @(posedge clk)
    clk_counter <= clk_counter + 1;

endmodule