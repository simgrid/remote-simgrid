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
        const std::function<void(void *)>& code, void * code_arg,
        std::function<void(void *)> before_connect_code = std::function<void(void *)>(), void * before_connect_code_arg = nullptr,
        std::function<void(void *)> after_connect_code = std::function<void(void *)>(), void * after_connect_code_arg = nullptr);
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
    void execute(double flop);
    void sleep_for(double duration);
    void sleep_until(double timeout);
    void yield();

    int get_pid();
    bool is_maestro();
}

}
