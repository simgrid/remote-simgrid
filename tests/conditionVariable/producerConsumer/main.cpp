#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/mutex.hpp"
#include "rsg/conditionVariable.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "../../../src/common.hpp"

#include <xbt.h>

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <inttypes.h>
#include <ctime>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::RsgService;
using namespace ::simgrid;

#define UNUSED(x) (void)(x)


#define NB_WORKER 10

struct s_consistency {
  simgrid::rsg::Mutex *g_lock; 
  simgrid::rsg::ConditionVariable *g_signal; 
  int allComplete;
};

// Worker function. Will compute some code and then notify the main thread.
static int Worker(void *params)
{
  struct s_consistency &cons = *(struct s_consistency*) params;
  XBT_INFO("worker begin...");
  
  // Generate a random number to compute
  int gen = 1;
  int waitFor = gen * 2500000;
  
  // Compute it
  simgrid::rsg::this_actor::execute(waitFor);
  XBT_INFO("Execute for [gen => %d] %d",gen, waitFor);
  
  XBT_INFO("worker finished");
  
  
  // finaly Acquire the cond mutex to notify the main thread which are waiting for all worker to complete
  // std::unique_lock<simgrid::rsg::Mutex> lock(*cons.g_lock);
  cons.g_lock->lock();
  // Increase de counter in safe zone
  (cons.allComplete)++;

  // Then notify a waitng thread
  cons.g_lock->unlock();
  cons.g_signal->notify_one();
  // cons.g_lock->unlock();
  rsg::this_actor::quit();  
  return 0;
}

// The Main function will Launch  a numbe rof worker and wait for all workers to finish
static int Main(void *)
{
  
  struct s_consistency *params = new  struct s_consistency(); 
  
  // The numbe of actor completed to 0
  params->allComplete = 0; 
  
  XBT_INFO("main running...");
  
  // The mutex for the ConditionVariable
  simgrid::rsg::Mutex g_lock;
  params->g_lock = &g_lock;
  
  // The condition Variable
  simgrid::rsg::ConditionVariable g_signal;
  params->g_signal = &g_signal;
  // For loop to create all workers
  for (int i = 0; i < NB_WORKER; i++) {   
    rsg::Actor::createActor("w1", simgrid::rsg::Host::current(), Worker, (void*) params);
  }
  
  XBT_INFO("main waiting for worker..." );
  
  // Wait all threads and avoid spurious wake-ups
  {
    while(params->allComplete != NB_WORKER) { 
      std::unique_lock<simgrid::rsg::Mutex> lock(g_lock);
      if(params->allComplete != NB_WORKER) {
        params->g_signal->wait(&g_lock);
        rsg::this_actor::sleep(0.1);
      }
    }
  }
  
  XBT_INFO("main finished...");
  g_lock.destroy();
  rsg::this_actor::quit();  
  return 0;
}

int main(int argc, char **argv) {
  rsg::HostPtr host1 = rsg::Host::by_name("host1");
  
  rsg::Actor* producer =  rsg::Actor::createActor("main" , host1 , Main, NULL);
  
  delete producer;
  
  rsg::this_actor::quit();  
  return 0; 
}
