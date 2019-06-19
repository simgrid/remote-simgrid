#pragma once

#include <memory>
#include <string>

namespace rsg
{

class Mailbox
{
private:
    Mailbox(const std::string & name);
    Mailbox(const Mailbox &) = delete;

public:
    static std::shared_ptr<Mailbox> by_name(const std::string & name);

    bool empty();
    bool listen();
    bool ready();

    void put(void * data, uint64_t size, uint64_t simulated_size);

    void * get();
    void * get(uint64_t & size);

    std::string get_name() const;
    const char * get_cname() const;
private:
    std::string _name;
};

}
