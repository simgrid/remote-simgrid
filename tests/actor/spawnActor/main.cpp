#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "../../../src/common.hpp"

#include "../../print.hpp"

#include <iostream>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <thread>
#include <inttypes.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

using namespace ::simgrid;

class PidComp {
  public:
  PidComp(std::string name) : pName(name) {}
  std::string pName;
  int operator()(void * ) {
    rsg::MailboxPtr mbox = rsg::Mailbox::byName(this->pName.c_str());
    int *pid = (int*) rsg::this_actor::recv(*mbox);
    int id = rsg::this_actor::getPid();
    RSG_INFO("PidComp -> [%d:%d] %d",id, *pid ,id - *pid);
    free(pid);
    rsg::this_actor::quit();
    return 1;
  }
};


int Spwaner(void * ) {
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  for(int i = 0; i < 100; i++) {
    std::string name = "PidComp" + std::to_string(i) + "__" + std::to_string(rsg::this_actor::getPid());
    rsg::MailboxPtr mbox = rsg::Mailbox::byName(name.c_str());

    rsg::Actor* actor = rsg::Actor::createActor("hello" , host1 , PidComp(name), NULL);
    int pid = actor->getPid();
    rsg::this_actor::send(*mbox, (char*) &(pid), sizeof(int));
    actor->join();
    delete actor;

  }

  rsg::this_actor::quit();
  return 1;
}

int main() {
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  for(int i = 0; i < 4; i++) {
    rsg::Actor* actor =  rsg::Actor::createActor("spawner" , host1 , Spwaner, NULL);
    actor->join();
    delete actor;
  }

  rsg::this_actor::sleep(1000);
  rsg::this_actor::quit();

  return 0;
}
