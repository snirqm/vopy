#include <iostream>
#include "app.h"
#include "easylogging++.h"


int main(int argc, char *argv[]) {
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "\033[1;32mSIM-CLIENT\033[0m %datetime %level : %msg");
    VOpyClientApp app(argc, argv);
    try { app.run(); } catch (const std::exception &e) {
        LOG(ERROR) << "Error: " << e.what();
        return 1;
    }
    return 0;

}