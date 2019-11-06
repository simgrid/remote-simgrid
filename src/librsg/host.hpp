#pragma once

#include <string>

#include "pointers.hpp"

namespace rsg
{
class Host
{
    friend class Actor;
private:
    explicit Host(const std::string & name);
    Host(const Host &) = delete;
    Host & operator=(const Host &) = delete;

public:
    static HostPtr by_name(const std::string & name);
    static HostPtr by_name_or_null(const std::string & name);
    static HostPtr current();

    std::string get_name() const;
    const char * get_cname() const;

private:
    std::string _name;
};

}
