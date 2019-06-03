#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    const char *msg = "Do you copy?";
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
    RSG_INFO("Send \"%s\" with size=%lu", msg, strlen(msg));

    char *rec;
    rec = rsg::this_actor::recv(*mbox);
    RSG_INFO("Received: \"%s\"", rec);

    rsg::MailboxPtr fooMb = rsg::Mailbox::byName("foo");
    rsg::this_actor::send(*fooMb, msg, strlen(msg) + 1);
    RSG_INFO("Send \"%s\" with size=%lu", msg, strlen(msg));

    char *fooRec;
    fooRec = rsg::this_actor::recv(*fooMb);
    RSG_INFO("Received: \"%s\"", fooRec);

    rsg::this_actor::quit();
    return 0;
}
