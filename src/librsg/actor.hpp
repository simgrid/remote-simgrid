#pragma once

#include <functional>
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

    static Actor* create(const std::string & name, const rsg::Host * host, const std::function<void()>& code);
    // TODO: propose the other create functions (https://simgrid.frama.io/simgrid/app_s4u.html#s4u-actor)

    Host* get_host();
    std::string get_name();
    int get_pid() const;

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
