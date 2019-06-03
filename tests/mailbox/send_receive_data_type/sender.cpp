#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

#include "struct.h"

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

  // char*
  const char *msg = "Do you copy ? ";
  rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
  RSG_INFO("send %s with size : %lu", msg, strlen(msg));

  //int
  int intMsg = -808080;
  rsg::this_actor::send(*mbox,(char*) &intMsg, sizeof(int));

  //int
  intMsg = 12;
  rsg::this_actor::send(*mbox,(char*) &intMsg, sizeof(int));

  //int[]
  int tabInt[12];
  for(int i = 0; i < intMsg; i++) {
    tabInt[i] = i*i;
  }
  rsg::this_actor::send(*mbox,(char*) &tabInt, intMsg * sizeof(int));

  structMsg strctMsg;
  strctMsg.intMsg = 123456789;
  strctMsg.msg[0] = 'B';
  strctMsg.msg[1] = 'a';
  strctMsg.msg[2] = 'l';
  strctMsg.msg[3] = 'r';
  strctMsg.msg[4] = 'o';
  strctMsg.msg[5] = 'g';
  strctMsg.msg[6] = '\0';
  rsg::this_actor::send(*mbox,(char*) &strctMsg, sizeof(strctMsg));

  rsg::this_actor::quit();
  return 0;
}
