#pragma once

#include <string>

#include "pointers.hpp"

namespace rsg
{

class Mailbox
{
private:
    explicit Mailbox(const std::string & name);
    Mailbox(const Mailbox &) = delete;
    Mailbox & operator=(const Mailbox &) = delete;

public:
    static MailboxPtr by_name(const std::string & name);

    bool empty();
    bool listen();
    bool ready();

    void put(void * data, uint64_t size, uint64_t simulated_size);
    CommPtr put_async(void * data, uint64_t size, uint64_t simulated_size);

    void * get();
    void * get(uint64_t & size);
    CommPtr get_async(void ** data);

    std::string get_name() const;
    const char * get_cname() const;
private:
    std::string _name;
};

}
