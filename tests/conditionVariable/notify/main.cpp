#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/mutex.hpp"
#include "rsg/conditionVariable.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"
#include "RsgMsg.hpp"

#include "xbt.h"

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <inttypes.h>
#include <ctime>


XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::RsgService;
using namespace ::simgrid;
#define UNUSED(x) (void)(x)

bool done = false;
#define NB_WORKER 1
#define MAX_ITER  1
#define SEED 1991

struct s_consistency {
  simgrid::rsg::Mutex *g_lock; 
  simgrid::rsg::ConditionVariable *g_signal; 
};

// Worker function. Will compute some code and then notify the main thread.
static int Worker(void *params)
{
  struct s_consistency &cons = *(struct s_consistency*) params;
  int nbIter = 0;

  XBT_INFO("worker begin...");
  UNUSED(cons);
  // Generate a random number to compute
  int gen = (1 + rand() % 10);
  int waitFor = gen * 2500000;
  
  // Compute it
  simgrid::rsg::this_actor::execute(waitFor);
  XBT_INFO("Execute for [gen => %d] %d",gen, waitFor);
  
  
  // Wait all threads and avoid spurious wake-ups
  {
    std::unique_lock<simgrid::rsg::Mutex> lock(*(cons.g_lock));
    while(nbIter != MAX_ITER) { 
      XBT_INFO("[THREAD]WAitings");
      cons.g_signal->wait_for(cons.g_lock,1);
      nbIter++;
    }
  }
  

  XBT_INFO("worker finished");
  rsg::this_actor::quit();  
  return 0;
}

int main(int argc, char **argv) {
  
  struct s_consistency *params = new  struct s_consistency(); 
  std::vector<rsg::Actor*> workers;
  XBT_INFO("main running...");
  
  // The mutex for the ConditionVariable
  simgrid::rsg::Mutex *g_lock = new simgrid::rsg::Mutex();
  params->g_lock = g_lock;
  
  // The condition Variable
  simgrid::rsg::ConditionVariable *g_signal = new simgrid::rsg::ConditionVariable();
  params->g_signal = g_signal;

    
  // finaly Acquire the cond mutex to notify the main thread which are waiting for all worker to complete
  {
    // For loop to create all workers
    for (int i = 0; i < NB_WORKER; i++) {   
      workers.push_back( rsg::Actor::createActor("w1", simgrid::rsg::Host::current(), Worker, (void*) params));
    }
    
    rsg::this_actor::sleep(1);  
    std::unique_lock<simgrid::rsg::Mutex> lock(*(params->g_lock));
    // Then notify a waitng thread
    params->g_signal->notify();
  }

  XBT_INFO("main finished [%d]", rsg::this_actor::getPid());

  // g_lock.destroy();
  rsg::this_actor::quit();   
  return 0; 
}
