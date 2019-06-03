#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/mutex.hpp"
#include "rsg/conditionVariable.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "../../print.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <inttypes.h>
#include <ctime>
#include <time.h>
#include <stdlib.h>

using namespace ::simgrid;

#define NB_WORKER 10
#define MAX_ITER  15

struct s_consistency {
  simgrid::rsg::Mutex *g_lock;
  simgrid::rsg::ConditionVariable *g_signal;
};

// Worker function. Will compute some code and then notify the main thread.
static int Worker(void *params)
{
  struct s_consistency &cons = *(struct s_consistency*) params;
  RSG_INFO("worker begin...");
  // Generate a random number to compute
  int gen = (1 + rand() % 10);
  int waitFor = gen * 2500000;

  // Compute it
  RSG_INFO("Execute for [gen => %d] %d",gen, waitFor);
  simgrid::rsg::this_actor::execute(waitFor);

  RSG_INFO("worker finished");

  // finaly Acquire the cond mutex to notify the main thread which are waiting for all worker to complete
  {
    std::unique_lock<simgrid::rsg::Mutex> lock(*cons.g_lock);
    // Then notify a waitng thread
    RSG_INFO("before notify");

    cons.g_signal->notify_one();
    RSG_INFO("after notify");
  }
  rsg::this_actor::quit();
  return 0;
}

// The Main function will Launch  a numbe rof worker and wait for all workers to finish
static int Main(void *)
{
  struct s_consistency *params = new  struct s_consistency();
  std::vector<rsg::Actor*> workers;
  int nbIter = 0;
  RSG_INFO("main running...");

  // The mutex for the ConditionVariable
  simgrid::rsg::Mutex g_lock;
  params->g_lock = &g_lock;

  // The condition Variable
  simgrid::rsg::ConditionVariable g_signal;
  params->g_signal = &g_signal;
  // For loop to create all workers
  for (int i = 0; i < NB_WORKER; i++) {
    workers.push_back( rsg::Actor::createActor("w1", simgrid::rsg::Host::current(), Worker, (void*) params));
  }

  RSG_INFO("main waiting for worker..." );

  // Wait all threads and avoid spurious wake-ups
  {
    std::unique_lock<simgrid::rsg::Mutex> lock(g_lock);
    while(nbIter != MAX_ITER) {
      std::cv_status status = params->g_signal->wait_for(&g_lock, 0.01);
      if(status == std::cv_status::timeout) {
        RSG_INFO("Wake up by timeout...");
      } else {
        RSG_INFO("Notified...");
      }
      nbIter++;
    }
  }

  for(std::vector<rsg::Actor*>::iterator it = workers.begin(); it != workers.end(); ++it) {
    (*it)->join();
  }

  RSG_INFO("main finished...");
  g_lock.destroy();
  rsg::this_actor::quit();
  return 0;
}

int main() {
  srand(1991);
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  rsg::Actor* producer =  rsg::Actor::createActor("main" , host1 , Main, NULL);
  producer->join();
  delete producer;

  rsg::this_actor::quit();
  return 0;
}
