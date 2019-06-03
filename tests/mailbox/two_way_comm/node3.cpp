#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    rsg::MailboxPtr fooMb = rsg::Mailbox::byName("foo");
    char *received = rsg::this_actor::recv(*fooMb);
    RSG_INFO("Received: \"%s\"", received);

    const char * sendMessage = "Ok";
    rsg::this_actor::send(*fooMb, sendMessage, strlen(sendMessage) + 1);

    rsg::this_actor::quit();
    return 0;
}
