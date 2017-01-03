#pragma once

#include <string>

#include "TZmqServer.hpp"

class RsgThriftServer {
public:
    RsgThriftServer(std::string& name);
    RsgThriftServer(const RsgThriftServer& other);//NEVER COPY A RsgThriftServer!
    int operator()();
    ~RsgThriftServer();
private:
    apache::thrift::server::TZmqServer* server;
    std::string name_;
};

