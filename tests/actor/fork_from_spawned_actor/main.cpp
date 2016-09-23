// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>
#define UNUSED(x) (void)(x)


XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::simgrid;

class ForkActor {
public:
    ForkActor() {}
    std::string pName;
    int operator()(void * ) {
        XBT_INFO("[PARENT]My pid is  : %d",rsg::this_actor::getPid());
        pid_t pid = rsg::this_actor::fork("child1");
        if(0 == pid) { // child
            pid_t pid_2 = rsg::this_actor::fork("child2");
            XBT_INFO("[child]My pid is  : %d",rsg::this_actor::getPid());
            XBT_INFO("[child]Fork returned : %d", pid);           
            XBT_INFO("[child]Fork returned : %d", pid_2);           
            rsg::this_actor::quit();
            return 0;
        } else {
            XBT_INFO("[child]My pid is  : %d",rsg::this_actor::getPid());
            XBT_INFO("[child]Fork returned : %d", pid);          
            rsg::this_actor::quit(); 
        }
        return 1;
    }
};


int main(int argc, char **argv) {
    // XBT_INFO("[parent]My id is  : %d",rsg::this_actor::getPid());

    rsg::HostPtr host1 = rsg::Host::by_name("host1");
    rsg::Actor *act = rsg::Actor::createActor("hello" , host1 , ForkActor(), NULL);
    act->join();
    // sleep(100);
    rsg::this_actor::quit();
    return 0;
}
