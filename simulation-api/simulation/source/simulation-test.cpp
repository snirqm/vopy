#include "simulation.h"


void run_test(bool (*test)(), char* test_name)
{

    if (test())
    {
        printf("\033[0;32m"); // this will make the text green
        printf("Test %s passed!\n", test_name);
        printf("\033[0m");
    }
    else
    {
        printf("\033[0;31m"); // this will make the text red
        printf("Test %s failed!\n", test_name);
        printf("\033[0m"); 
    }
}

bool test_simple_simulation()
{
    unsigned char readBuffer[256];
    unsigned char writeBuffer[256];

    // Clear readBuffer
    memset(readBuffer, 0, 256);

    // Read initial memory contents
    read_task(0, 256, readBuffer);
    printf("Initial memory contents:\n");
    for (int i = 0; i < 256; i++) {
        printf("%3d ", readBuffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    // Prepare write buffer
    for (int i = 0; i < 256; i++) {
        writeBuffer[i] = i;
    }

    // Write to memory
    write_task(0, 256, writeBuffer);

    // Clear readBuffer again
    memset(readBuffer, 0, 256);

    // Read back from memory
    read_task(0, 256, readBuffer);
    printf("Memory contents after write:\n");
    for (int i = 0; i < 256; i++) {
        printf("%3d ", readBuffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    // Check results
    for (int i = 0; i < 256; i++) {
        if (readBuffer[i] != (unsigned char)i) {
            printf("Mismatch at index %d: expected %d, got %d\n", i, i, readBuffer[i]);
            return false;
        }
    }

    return true;
}


int entrypoint()
{
    printf("starting simple simulation test\n");
    run_test(test_simple_simulation, "simple simulation");
    exit(0);
}
