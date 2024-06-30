
#include <iostream>
#include "app.h"


int main(int argc, char *argv[]) {
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "\033[1;34mSIM-SERVER\033[0m %datetime %level : %msg");
    try { VOpyServerApp(argc, argv).run(); } catch (const std::exception &e) {
        LOG(ERROR) << "catched exception: " << e.what();
        return 1;
    }
    return 0;
}