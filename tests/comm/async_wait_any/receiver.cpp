#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");


#define nbWorker 10

using boost::shared_ptr;
using namespace ::simgrid;

int Worker(void* arg) {
  int *msg = (int*) malloc(sizeof(int));
  *msg = *(int*) arg * 10;
  rsg::this_actor::sleep(1);
  rsg::Mailbox *mbox = rsg::Mailbox::byName("test");
  rsg::this_actor::send(*mbox, (char*) msg, sizeof(int));
  rsg::this_actor::quit();  
  free(msg);
  return 0;
}

int main(int argc, char **argv) {
  int *mboxbuff[10];
  std::vector<rsg::Comm*> comms;
  rsg::Mailbox *mb = rsg::Mailbox::byName("test");

  for(int i = 0 ; i <  nbWorker; i++) {    
    rsg::Comm *comm = &rsg::Comm::recv_init(*mb);
    comm->setDstData((void**)&(mboxbuff[i]), sizeof(void*));
    comms.push_back(comm);
    comm->start();
    
    rsg::Actor::createActor("worker", rsg::Host::by_name("host1"), Worker, (void*) &i);
  }
  
  
  for(int i = 0 ; i <  nbWorker; i++) {    
    auto it = rsg::Comm::wait_any(comms.begin(), comms.end());
    comms.erase(it);
    if(i == 0 ) {
      int pos = it - comms.begin();
      std::cout << "results " << *mboxbuff[pos] << " at pos " << pos << std::endl;
    }
  }
  
  for(int i = 0 ; i <  nbWorker; i++) {    
    printf("%d\n", *mboxbuff[i]);
  }
  
  rsg::this_actor::quit();
  return 0;
}
