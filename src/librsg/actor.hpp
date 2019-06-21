#pragma once

#include <functional>
#include <memory>
#include <string>

namespace rsg
{
class Host;

class Actor
{
private:
    explicit Actor(int id);
    Actor(const Actor &) = delete;
    Actor & operator=(const Actor &) = delete;

public:
    static std::shared_ptr<Actor> self();

    static std::shared_ptr<Actor> create(const std::string & name, const std::shared_ptr<Host> & host,
        const std::function<void(void *)>& code, void * code_data);
    // TODO: propose the other create functions (https://simgrid.frama.io/simgrid/app_s4u.html#s4u-actor)

    std::shared_ptr<Host> get_host();
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
