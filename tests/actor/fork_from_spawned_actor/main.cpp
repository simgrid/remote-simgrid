#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

class ForkActor {
public:
    ForkActor() {}
    std::string pName;
    int operator()(void * ) {
        RSG_INFO("My pid is %d",rsg::this_actor::getPid());
        pid_t pid = rsg::this_actor::fork("child1");
        if(0 == pid) { // child
            pid_t pid_2 = rsg::this_actor::fork("child2");
            RSG_INFO("My pid is %d",rsg::this_actor::getPid());
            RSG_INFO("Fork returned %d", pid);
            RSG_INFO("Fork returned %d", pid_2);
            rsg::this_actor::quit();
            return 0;
        } else {
            RSG_INFO("My pid is %d",rsg::this_actor::getPid());
            RSG_INFO("Fork returned %d", pid);
            rsg::this_actor::quit();
        }
        return 1;
    }
};


int main() {
    rsg::HostPtr host1 = rsg::Host::by_name("host1");
    rsg::Actor *act = rsg::Actor::createActor("hello" , host1 , ForkActor(), NULL);
    act->join();
    delete act;

    rsg::this_actor::quit();
    return 0;
}
