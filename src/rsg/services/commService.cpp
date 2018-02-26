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

    comms->insert({reinterpret_cast<uintptr_t>(&(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&(cd->ptr));
}

int64_t rsg::RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::recv_init(mbox);

    comms->insert({reinterpret_cast<uintptr_t>(&(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&(cd->ptr));
}

int64_t rsg::RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
    
    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::recv_init(mbox);

    cd->buffer = (void**) malloc(sizeof(void*));
    cd->ptr->setDstData(cd->buffer, sizeof(void*));

    comms->insert({reinterpret_cast<uintptr_t>(&(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&(cd->ptr));
}

int64_t rsg::RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int64_t size, const int64_t simulatedByteAmount) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);
    std::string *strData = new std::string(data.data(), data.length());

    CommData * cd = new CommData;
    cd->ptr = s4u::Comm::send_async(mbox, (void*) strData, simulatedByteAmount);

    comms->insert({reinterpret_cast<uintptr_t>(&(cd->ptr)), cd});

    return reinterpret_cast<int64_t>(&(cd->ptr));
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
    std::string *payload = new std::string(buff.data(), buff.length());
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

            delete res;
            free(cd->buffer);
            cd->buffer = nullptr;
        }
    } else {
        _return.data.assign("", 0);
        _return.cond = false;
    }
}

void rsg::RsgCommHandler::wait_any(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms) {
    std::vector<s4u::CommPtr> s4uComms;
    for(auto comm_int : comms) {
        s4uComms.push_back(this->comms->at((uintptr_t) comm_int)->ptr);
    }

    _return.index = (int64_t) s4u::Comm::wait_any(&s4uComms);
    debug_server_print("AHOY. Wait any finished, index=%ld (comms.size()=%zu)",
                       _return.index, comms.size());

    CommData * cd = this->comms->at((uintptr_t) &*(s4uComms.at(_return.index)));

    if (cd->buffer != nullptr) {
        std::string * res = (std::string*) *(cd->buffer);
        _return.data.assign(res->data(), res->length());

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
        return;
    }

    CommData * cd = this->comms->at((uintptr_t) &*(s4uComms.at(_return.index)));

    if (cd->buffer != nullptr) {
        std::string * res = (std::string*) *(cd->buffer);
        _return.data.assign(res->data(), res->length());

        delete res;
        free(cd->buffer);
        cd->buffer = nullptr;
    }
}

