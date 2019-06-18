#pragma once

#include <memory>
#include <string>

namespace rsg
{
class Actor;

class Host
{
    friend class Actor;
private:
    Host(const std::string & name);
    Host(const Host &) = delete;

public:
    static std::shared_ptr<Host> by_name(const std::string & name);
    static std::shared_ptr<Host> by_name_or_null(const std::string & name);

    std::string get_name() const;

private:
    std::string _name;
};

bool operator==(const Host & a, const Host & b);

}
