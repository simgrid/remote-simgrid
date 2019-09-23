#pragma once

#include <functional>
#include <memory>
#include <string>

#include "pointers.hpp"

namespace rsg
{

class Actor
{
private:
    explicit Actor(int id);
    Actor(const Actor &) = delete;
    Actor & operator=(const Actor &) = delete;

public:
    static ActorPtr self();

    static ActorPtr create(const std::string & name, const HostPtr & host,
        const std::function<void(void *)>& code, void * code_data);
    // TODO: propose the other create functions (https://simgrid.frama.io/simgrid/app_s4u.html#s4u-actor)

    static int fork(const std::string & child_name, const HostPtr & host); // calls system's fork(). return value is POXIX with actor ids.

    HostPtr get_host();
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
