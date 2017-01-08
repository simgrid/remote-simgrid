#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/mutex.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>
#include <iostream>
#include <thread>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::simgrid;

rsg::Mutex mtx;

class Add {
int pVal;
public:
  
  Add(int val) : pVal(val) {};
  int operator()(void *res) {
    XBT_INFO("Hello i will add %d", pVal);
    mtx.lock();
    int *sharedRes = (int*)res;
    *sharedRes += pVal;
    mtx.unlock();
    rsg::this_actor::quit();
    return 1;
  };
  
};

int main(int argc, char **argv) {

  rsg::HostPtr host1 = rsg::Host::by_name("host1");
  
  int * results = new int;
  *results = 0;
  for (int i = 0; i <  10; i++) {  
    rsg::Actor::createActor("receiver" , host1, Add(i), results);
  }
  
  rsg::this_actor::sleep(10000);
  XBT_INFO("Results is : %d ", *results);
  
  mtx.destroy();
  rsg::this_actor::quit();
  return 0;
}
