#include <iostream>
#include "app.hpp"



int main(int argc, char *argv[]) {
    VOpyClientApp app(argc, argv);
    auto code = app.run();
    std::cout << "Exiting with code: " << code << std::endl;
    return code;

}