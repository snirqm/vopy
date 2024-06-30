
module my_test;
    `define MEMORY_SIZE 1024
    `define REG_SIZE 32
    
    import "DPI-C" context task entrypoint();
    import "DPI-C" context task server_entrypoint();

    // Declare and export the functions and tasks
    export "DPI-C" function get_result;
    export "DPI-C" task read_task;
    export "DPI-C" task write_task;
    export "DPI-C" task sim_clock_task;
    export "DPI-C" task sim_to_last_task;
    export "DPI-C" task exit_task;
    
    logic clk;
    int unsigned clk_counter;
    int unsigned port_number;
    bit finish_test = 0;
    int unsigned result;
    
    logic [`REG_SIZE - 1:0] memory [0:`MEMORY_SIZE - 1];
    
    initial begin
        for (int i = 0; i < `MEMORY_SIZE; i++) begin
            memory[i] = 0;
        end
    end

    function int get_result();
        return result;
    endfunction


    task read_task(input int a);
        repeat (10) @(posedge clk);
        if (a >= 0 && a < `MEMORY_SIZE) begin
            result = memory[a];
        end else begin
            result = -1;
        end
    endtask

    task write_task(input int a, input int b);
        repeat (10) @(posedge clk);
        if (a >= 0 && a < `MEMORY_SIZE) begin
            memory[a] = b;
            result = 1;
        end else begin
            result = 0;
        end
    endtask

    task sim_clock_task(input int clock_num);
        repeat (clock_num) @(posedge clk);
        result = clk_counter;
    endtask

    task sim_to_last_task();
        repeat (100) @(posedge clk);
        finish_test = 1;
        result = 1;
    endtask

    task exit_task();
        repeat (10) @(posedge clk);
        finish_test = 1;
        result = 1;
    endtask

    initial begin
        while (finish_test == 0) begin 
            server_entrypoint();
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
