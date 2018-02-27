#include <simgrid/s4u.hpp>
#include <iostream>

#include "../services.hpp"

using namespace ::apache::thrift::server;
using namespace ::RsgService;
using namespace ::simgrid;

std::unordered_map<uintptr_t, rsg::CommData*> *rsg::RsgCommHandler::comms = new std::unordered_map<uintptr_t, rsg::CommData*>();

rsg::RsgCommHandler::RsgCommHandler() {
}

int64_t rsg::RsgCommHandler::send_init(const int64_t sender, const int64_t dest) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);

    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::send_init(mbox);

    comms->insert({reinterpret_cast<uintptr_t>(&*(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&*(cd->ptr));
}

int64_t rsg::RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::recv_init(mbox);

    comms->insert({reinterpret_cast<uintptr_t>(&*(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&*(cd->ptr));
}

int64_t rsg::RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::recv_init(mbox);

    cd->buffer = (void**) malloc(sizeof(void*));
    cd->ptr->setDstData(cd->buffer, sizeof(void*));

    comms->insert({reinterpret_cast<uintptr_t>(&*(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&*(cd->ptr));
}

int64_t rsg::RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int64_t size, const int64_t simulatedByteAmount) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);

    if ((size_t)size > data.length())
    {
        fprintf(stderr, "Invalid call to rsg::send_async. Given size (%ld) is invalid (data size is %zu).",
                size, data.length());
        _exit(1);
    }

    std::string *strData = new std::string(data.data(), size);

    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::send_async(mbox, (void*) strData, simulatedByteAmount);

    comms->insert({reinterpret_cast<uintptr_t>(&*(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&*(cd->ptr));
}

void rsg::RsgCommHandler::start(const int64_t addr) {
    CommData * cd = comms->at((uintptr_t) addr);
    cd->ptr->start();
}

void rsg::RsgCommHandler::waitComm(std::string& _return, const int64_t addr) {
    CommData * cd = comms->at((uintptr_t) addr);
    cd->ptr->wait();

    if (cd->buffer != nullptr) {
        std::string * res = (std::string*) *(cd->buffer);
        _return.assign(res->data(), res->length());
        debug_server_print("AHOY. Set comm result from %p (in waitComm)", res);

        delete res;
        free(cd->buffer);
        cd->buffer = nullptr;
    }
}

void rsg::RsgCommHandler::setSrcDataSize(const int64_t addr, const int64_t size) {
    CommData * cd = comms->at((uintptr_t) addr);
    cd->ptr->setSrcDataSize((size_t) size);
}

int64_t rsg::RsgCommHandler::getDstDataSize(const int64_t addr) {
    CommData * cd = comms->at((uintptr_t) addr);
    return cd->ptr->getDstDataSize();
}

void rsg::RsgCommHandler::setRate(const int64_t addr, const double rate) {
    CommData * cd = comms->at((uintptr_t) addr);
    cd->ptr->setRate(rate);
}

void rsg::RsgCommHandler::setSrcData(const int64_t addr, const std::string& buff) {
    CommData * cd = comms->at((uintptr_t) addr);
    std::string *payload = new std::string(buff.data(), buff.length()); // leak on multiple call
    cd->ptr->setSrcData((void*)payload, sizeof(void*));
}

void rsg::RsgCommHandler::setDstData(const int64_t addr) { //FIXME USE THE SIZE
    CommData * cd = comms->at((uintptr_t) addr);
    xbt_assert(cd->buffer == nullptr, "Comm already has a destination buffer");

    cd->buffer = (void**) malloc(sizeof(void*));
    cd->ptr->setDstData(cd->buffer, sizeof(void*));
}

void rsg::RsgCommHandler::test(rsgCommBoolAndData& _return, const int64_t addr) {
    CommData * cd = comms->at((uintptr_t) addr);
    if(cd->ptr->test()) {
        _return.cond = true;

        if (cd->buffer != nullptr) {
            std::string * res = (std::string*) *(cd->buffer);
            _return.data.assign(res->data(), res->length());
            debug_server_print("AHOY. Set comm result from %p (in test)", res);

            delete res;
            free(cd->buffer);
            cd->buffer = nullptr;
        }
    } else {
        _return.data.assign("", 0);
        _return.cond = false;
        debug_server_print("AHOY. Set comm result as null (test failed)");
    }
}

void rsg::RsgCommHandler::wait_any(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms) {
    std::vector<s4u::CommPtr> s4uComms;
    for(auto comm_int : comms) {
        s4uComms.push_back(this->comms->at((uintptr_t) comm_int)->ptr);
    }

    _return.index = (int64_t) s4u::Comm::wait_any(&s4uComms);
    debug_server_print("AHOY. Wait any finished, index=%ld (comms.size()=%zu)", _return.index, comms.size());

    debug_server_print("AHOY. s4uComms size=%zu", s4uComms.size());
    s4u::CommPtr finishedCommPtr = s4uComms.at(_return.index);
    debug_server_print("AHOY. Could retrieve CommPtr at index %zu.", _return.index);

    uintptr_t finishedCommIntptr = reinterpret_cast<uintptr_t>(&*(finishedCommPtr));
    debug_server_print("AHOY. Could retrieve uintptr_t");

    CommData * cd = this->comms->at(finishedCommIntptr);
    debug_server_print("AHOY. Could retrieve CommData!");

    if (cd->buffer != nullptr) {
        std::string * res = (std::string*) *(cd->buffer);
        _return.data.assign(res->data(), res->length());
        debug_server_print("AHOY. Set comm result from %p (in wait_any)", res);

        delete res;
        free(cd->buffer);
        cd->buffer = nullptr;
    }
}

void rsg::RsgCommHandler::wait_any_for(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms, const double timeout) {
    std::vector<s4u::CommPtr> s4uComms;
    for(auto comm_int : comms) {
        s4uComms.push_back(this->comms->at((uintptr_t) comm_int)->ptr);
    }

    _return.index = (int64_t) s4u::Comm::wait_any_for(&s4uComms, timeout);

    // If timeout reached
    if (_return.index < 0 || _return.index >= (int64_t) s4uComms.size())
    {
        _return.index = -1;
        _return.data.assign("", 0);
        debug_server_print("AHOY. Set comm result as null (wait_any_for, timeout reached)");
        return;
    }

    CommData * cd = this->comms->at((uintptr_t) &*(s4uComms.at(_return.index)));

    if (cd->buffer != nullptr) {
        std::string * res = (std::string*) *(cd->buffer);
        _return.data.assign(res->data(), res->length());
        debug_server_print("AHOY. Set comm result from %p (in wait_any_for)", res);

        delete res;
        free(cd->buffer);
        cd->buffer = nullptr;
    }
}

