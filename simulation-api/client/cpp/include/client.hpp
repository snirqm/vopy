#pragma once
#include <string>
class VOpyClient {
    public:
    void connect() {}
};

class FileSimulationClient : public VOpyClient {
public:
    FileSimulationClient(const std::string &in_path, const std::string &out_path) {}
};

class TcpSimulationClient : public VOpyClient {
public:
    TcpSimulationClient(int port) {}
};