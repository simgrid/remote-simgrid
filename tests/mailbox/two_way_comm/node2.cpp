#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    char *received = rsg::this_actor::recv(*mbox);
    RSG_INFO("Received from client: \"%s\" with size=%lu", received, strlen(received) );

    const char * sendMessage = "Ok";
    rsg::this_actor::send(*mbox, sendMessage, strlen(sendMessage) + 1);
    RSG_INFO("Received: \"%s\"", sendMessage);

    rsg::this_actor::quit();
    return 0;
}
