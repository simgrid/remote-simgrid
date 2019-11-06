#include <librsg.hpp>

void sleeper(void *)
{
  RSG_INFO("Sleeper started");
  rsg::this_actor::sleep_for(3);
  RSG_INFO("I'm done. See you!");
}

void master()
{
  rsg::ActorPtr actor;

  RSG_INFO("Start sleeper");
  actor = rsg::Actor::create("sleeper1", rsg::Host::current(), sleeper, nullptr);
  RSG_INFO("Join the sleeper (timeout 2)");
  actor->join(2);

  RSG_INFO("Start sleeper");
  actor = rsg::Actor::create("sleeper2", rsg::Host::current(), sleeper, nullptr);
  RSG_INFO("Join the sleeper (timeout 4)");
  actor->join(4);

  RSG_INFO("Start sleeper");
  actor = rsg::Actor::create("sleeper3", rsg::Host::current(), sleeper, nullptr);
  RSG_INFO("Join the sleeper (timeout 2)");
  actor->join(2);

  RSG_INFO("Start sleeper");
  actor = rsg::Actor::create("sleeper4", rsg::Host::current(), sleeper, nullptr);
  RSG_INFO("Waiting 4");
  rsg::this_actor::sleep_for(4);
  RSG_INFO("Join the sleeper after its end (timeout 1)");
  try { actor->join(1); } catch (...) { RSG_INFO("exception caught. should not happen, FIXME");};

  rsg::this_actor::sleep_for(1);
  RSG_INFO("Goodbye now!");
}

int main(int argc, char* argv[])
{
  master();

  return 0;
}
