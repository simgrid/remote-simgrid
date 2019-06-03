#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "../../print.hpp"

using namespace ::simgrid;


int actor(void *)
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    RSG_INFO("%10.9f <- clock before receive (child)", rsg::getClock());
    rsg::this_actor::recv(*mbox);
    RSG_INFO("%10.9f <- clock after receive (child)", rsg::getClock());
    rsg::this_actor::quit();
    return 1;
}

int main() {
    const char *msg = "Do you copy ?";
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    RSG_INFO("%10.9f <- clock before execute", rsg::getClock());
    rsg::this_actor::execute(8095000000 * 2);
    RSG_INFO("%10.9f <- clock after execute", rsg::getClock());

    rsg::Actor *act = rsg::Actor::createActor("receiver" , host1 , actor, NULL);

    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    RSG_INFO("%10.9f <- clock before send", rsg::getClock());
    rsg::this_actor::send(*mbox,msg, strlen(msg) + 1, 115476000);
    RSG_INFO("%10.9f <- clock after send", rsg::getClock());
    act->join();
    rsg::this_actor::quit();
    return 0;
}

