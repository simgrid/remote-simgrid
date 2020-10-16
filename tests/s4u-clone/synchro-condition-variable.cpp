#include <librsg.hpp>

#include <mutex>           /* std::mutex and std::lock_guard */

std::string data;
bool done = false;

struct WorkerArgs {
  rsg::ConditionVariablePtr cv = nullptr;
  rsg::MutexPtr mutex = nullptr;
};

static void worker_fun(void * args)
{
  auto worker_args = (struct WorkerArgs *) args;
  std::unique_lock<rsg::Mutex> lock(*worker_args->mutex);

  RSG_INFO("Start processing data which is '%s'.", data.c_str());
  data += std::string(" after processing");

  // Send data back to main()
  RSG_INFO("Signal to master that the data processing is completed, and exit.");

  done = true;
  worker_args->cv->notify_one();
}

static void wait_worker(rsg::ConditionVariablePtr cv, rsg::MutexPtr mutex)
{
  std::unique_lock<rsg::Mutex> lock(*mutex);
  cv->wait(mutex);
}

int main(int argc, char** argv)
{
  struct WorkerArgs worker_args;
  worker_args.cv = rsg::ConditionVariable::create();
  worker_args.mutex = rsg::Mutex::create();
  data = std::string("Example data");

  auto worker = rsg::Actor::create("worker", rsg::Host::by_name("Jupiter"), worker_fun, &worker_args);

  wait_worker(worker_args.cv, worker_args.mutex);
  RSG_INFO("data is now '%s'.", data.c_str());

  worker->join();
  return 0;
}
