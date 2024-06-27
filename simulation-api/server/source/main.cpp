#include "easylogging++.h"
#include <iostream>
#include "app.h"


int main(int argc, char *argv[]) {
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "\033[1;34mSIM-SERVER\033[0m %datetime %level : %msg");
    VOpyServerApp app(argc, argv);
    try { app.run(); } catch (const std::exception &e) {
        LOG(ERROR) << "Error: " << e.what();
        return 1;
    }
    return 0;
}