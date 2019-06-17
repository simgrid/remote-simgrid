#pragma once

#include <string>

namespace rsg
{
class Host;

class Actor
{
private:
    Actor(int id);
    Actor(const Actor &) = delete;

public:
    static Actor* self();

    Host* get_host();
    std::string get_name();
    int get_pid();

private:
    int _id;
};

namespace this_actor
{
    bool is_maestro();
    void sleep_for(double duration);
    void sleep_until(double timeout);
    int get_pid();
}

}
