#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

#include "struct.h"

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);
  RSG_INFO("Received : %s with size of %lu", received, strlen(received) );

  int *recInt = (int*) rsg::this_actor::recv(*mbox);
  RSG_INFO("Received : %d", *recInt);
  free(recInt);

  recInt = (int*) rsg::this_actor::recv(*mbox);
  RSG_INFO("Received : %d", *recInt);

  int *recTabInt;
  recTabInt = (int*) rsg::this_actor::recv(*mbox);
  for(int i = 0; i < *recInt ;i++) {
    RSG_INFO(" tab[%d] = %d", i , recTabInt[i]);
  }
  free(recTabInt);
  free(recInt);

  structMsg *recStruct = (structMsg*) rsg::this_actor::recv(*mbox);
  RSG_INFO(" recStruct->intMsg = %d", recStruct->intMsg);
  RSG_INFO(" recStruct->msg = %s", recStruct->msg);
  free(recStruct);
  rsg::this_actor::quit();
  return 0;
}
