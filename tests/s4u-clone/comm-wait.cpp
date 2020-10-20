#include <cstdlib>
#include <iostream>
#include <string>

#include <librsg.hpp>

static void sender(void *)
{
  const long messages_count = 4;
  const long msg_size = 482117300;
  double sleep_start_time = 5.0;
  double sleep_test_time  = 0.5;

  auto mbox = rsg::Mailbox::by_name("receiver");

  RSG_INFO("Sleeping for %g s", sleep_start_time);
  rsg::this_actor::sleep_for(sleep_start_time);

  for (int i = 0; i < messages_count; i++) {
    std::string msg = std::string("Message ") + std::to_string(i);

    RSG_INFO("Sending '%s' to '%s' (%ld bytes)", msg.c_str(), mbox->get_cname(), msg_size);
    auto comm = mbox->put_async((void*)msg.data(), msg.size() + 1, msg_size);

    if (i % 2 == 0) {
      while (not comm->test()) {
        RSG_INFO("Message not fully sent yet. Sleeping %g s", sleep_test_time);
        rsg::this_actor::sleep_for(sleep_test_time);
      }
    } else {
      comm->wait();
    }
  }

  /* Send message to let the receiver know that it should stop */
  RSG_INFO("Sending 'finalize' to 'receiver'");
  std::string msg = "finalize";
  mbox->put((void*)msg.data(), msg.size() + 1, 4);
}

static void receiver(void *)
{
  double sleep_start_time = 1.0;
  double sleep_test_time  = 0.7;

  auto mbox = rsg::Mailbox::by_name("receiver");

  RSG_INFO("Sleeping for %g s", sleep_start_time);
  rsg::this_actor::sleep_for(sleep_start_time);

  bool cont = true;
  for (int i = 0; cont; i++) {
    char * msg = nullptr;
    RSG_INFO("Waiting for message #%d", i);
    auto comm = mbox->get_async((void**) &msg);

    if (i % 2 == 1) {
      while (not comm->test()) {
        RSG_INFO("Message not fully received yet. Sleeping %g s", sleep_test_time);
        rsg::this_actor::sleep_for(sleep_test_time);
      }
    } else {
      comm->wait();
    }

    RSG_INFO("Received '%s'", msg);
    if (std::string(msg) == "finalize")
      cont = false; // If it's a finalize message, we're done.
    delete[] msg;
  }
}

int main()
{
  auto actor = rsg::Actor::self();
  auto host = actor->get_host();

  rsg::Actor::create("sender", host, sender, nullptr);
  rsg::Actor::create("receiver", host, receiver, nullptr);

  return 0;
}
