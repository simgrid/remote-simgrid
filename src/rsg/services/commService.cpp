#include <simgrid/s4u.hpp>
#include <iostream>

#include "../services.hpp"


using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;


boost::unordered_map<const int64_t ,unsigned long int> *rsg::RsgCommHandler::buffers = new boost::unordered_map<const int64_t ,unsigned long int>();

rsg::RsgCommHandler::RsgCommHandler() {
}

int64_t rsg::RsgCommHandler::send_init(const int64_t sender, const int64_t dest) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);

    auto comm = s4u::Comm::send_init(mbox);
    return reinterpret_cast<int64_t>(&comm);
}

int64_t rsg::RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    auto comm = s4u::Comm::recv_init(mbox);
    return reinterpret_cast<int64_t>(&comm);
}

int64_t rsg::RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    unsigned long int bufferAddr;
    unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
    bufferAddr = ptr;
    auto comm = s4u::Comm::recv_init(mbox);
    comm->setDstData((void**) bufferAddr, sizeof(std::string*));
    buffers->insert({(int64_t) &comm, (unsigned long int) bufferAddr});

    return reinterpret_cast<int64_t>(&comm);
}

int64_t rsg::RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int64_t size, const int64_t simulatedByteAmount) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);
    std::string *strData = new std::string(data.data(), data.length());

    auto comm = s4u::Comm::send_async(mbox, (void*) strData, simulatedByteAmount);
    return reinterpret_cast<int64_t>(&comm);
}

void rsg::RsgCommHandler::start(const int64_t addr) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    comm->start();
}

void rsg::RsgCommHandler::waitComm(std::string& _return, const int64_t addr) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    comm->wait();
    try {
        void **buffer = (void**) buffers->at((unsigned long int)addr);
        if(buffer) {
            std::string *res = (std::string*) *buffer;
            _return.assign(res->data(), res->length());
            delete res;
            free(buffer);
            buffers->erase(addr);
        } else {
            xbt_die("Empty dst buffer");
        }
    } catch (std::out_of_range& e) {
        
    }
}

void rsg::RsgCommHandler::setSrcDataSize(const int64_t addr, const int64_t size) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    comm->setSrcDataSize((size_t)size);
}

int64_t rsg::RsgCommHandler::getDstDataSize(const int64_t addr) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    return comm->getDstDataSize();
}

void rsg::RsgCommHandler::setRate(const int64_t addr, const double rate) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    comm->setRate(rate);
}

void rsg::RsgCommHandler::setSrcData(const int64_t addr, const std::string& buff) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    std::string *payload = new std::string(buff.data(), buff.length());
    comm->setSrcData((void*)payload, sizeof(void*));
}

void rsg::RsgCommHandler::setDstData(const int64_t addr) { //FIXME USE THE SIZE
    s4u::Comm *comm = (s4u::Comm*) addr;
    unsigned long int bufferAddr;
    unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
    bufferAddr = ptr;
    comm->setDstData((void**) bufferAddr, sizeof(std::string*));
    buffers->insert({addr, bufferAddr});
}

void rsg::RsgCommHandler::test(rsgCommBoolAndData& _return, const int64_t addr) {
    s4u::Comm *comm = (s4u::Comm*) addr;
    if(comm->test()) {
        _return.cond = true;
        if (buffers->find((unsigned long int)addr) != buffers->end()) {
            void **buffer = (void**) buffers->at((unsigned long int)addr);
            if(buffer) {
                std::string *res = (std::string*) *buffer;
                _return.data.assign(res->data(), res->length());
                delete res;
                free(buffer);
                buffers->erase(addr);
            }
        }
    } else {    
        _return.data.assign("", 0);
        _return.cond = false;
    }
}

void rsg::RsgCommHandler::wait_any(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms) {
    std::vector<s4u::Comm*> s4uComms;
    for(auto it = comms.begin(); it != comms.end(); it++) {
        s4u::Comm *comm = (s4u::Comm*) *it;
        s4uComms.push_back(comm);
    }
    auto commResIt = s4u::Comm::wait_any(s4uComms.begin(), s4uComms.end());
    
    _return.index = commResIt - s4uComms.begin();
    
    if (buffers->find((unsigned long int)*commResIt) != buffers->end()) {
        void **buffer = (void**) buffers->at((unsigned long int)*commResIt);
        if(buffer) {
            std::string *res = (std::string*) *buffer;
            _return.data.assign(res->data(), res->length());
            delete res;
            free(buffer);
            buffers->erase((int64_t)*commResIt);
        }
    }
}

void rsg::RsgCommHandler::wait_any_for(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms, const double timeout) {
    std::vector<s4u::Comm*> s4uComms;
    for(auto it = comms.begin(); it != comms.end(); it++) {
        s4u::Comm *comm = (s4u::Comm*) *it;
        s4uComms.push_back(comm);
    }
    auto commResIt = s4u::Comm::wait_any_for(s4uComms.begin(), s4uComms.end(), timeout);
    if(commResIt == s4uComms.end()) {
        _return.index = - 1;
        _return.data.assign("", 0);
        return;
    }
    
    _return.index = commResIt - s4uComms.begin();
    
    if (buffers->find((unsigned long int)*commResIt) != buffers->end()) {
        void **buffer = (void**) buffers->at((unsigned long int)*commResIt);
        if(buffer) {
            std::string *res = (std::string*) *buffer;
            _return.data.assign(res->data(), res->length());
            delete res;
            free(buffer);
            buffers->erase((int64_t)*commResIt);
        }
    }
}

