#include <librsg.hpp>

#include <mutex> /* std::mutex and std::lock_guard */
#include "simgrid/s4u.hpp" /* All of S4U */

constexpr int NB_ACTOR = 6;

struct actor_args {
  rsg::MutexPtr mutex = nullptr;
  int result = 0;
};

/* This worker uses a classical mutex */
static void worker(void * args)
{
  auto worker_args = (struct actor_args *) args;
  // lock the mutex before enter in the critical section
  worker_args->mutex->lock();

  RSG_INFO("Hello s4u, I'm ready to compute after a regular lock");
  // And finaly add it to the results
  worker_args->result += 1;
  RSG_INFO("I'm done, good bye");

  // You have to unlock the mutex if you locked it manually.
  // Beware of exceptions preventing your unlock() from being executed!
  worker_args->mutex->unlock();
}

static void workerLockGuard(void * args)
{
  auto worker_args = (struct actor_args *) args;
  // Simply use the std::lock_guard like this
  // It's like a lock() that would do the unlock() automatically when getting out of scope
  std::lock_guard<rsg::Mutex> lock(*worker_args->mutex);

  // then you are in a safe zone
  RSG_INFO("Hello s4u, I'm ready to compute after a lock_guard");
  // update the results
  worker_args->result += 1;
  RSG_INFO("I'm done, good bye");

  // Nothing specific here: the unlock will be automatic
}

static void master(void * args)
{
  struct actor_args worker_args;
  worker_args.mutex = rsg::Mutex::create();

  for (int i = 0; i < NB_ACTOR * 2 ; i++) {
    // To create a worker use the static method rsg::Actor.
    if((i % 2) == 0 )
      rsg::Actor::create("worker", rsg::Host::by_name("Jupiter"), workerLockGuard, (void *) &worker_args);
    else
      rsg::Actor::create("worker", rsg::Host::by_name("Tremblay"), worker, (void *) &worker_args);
  }

  rsg::this_actor::sleep_for(10);
  RSG_INFO("Results is -> %d", worker_args.result);
}

int main(int argc, char **argv)
{
  rsg::Actor::create("main", rsg::Host::by_name("Tremblay"), master, nullptr);
  return 0;
}
