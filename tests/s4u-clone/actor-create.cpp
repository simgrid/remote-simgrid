#include <string>

#include <librsg.hpp>

struct SenderArgs { std::string mbox; std::string msg; };
void sender(void * void_args)
{
    auto args = (SenderArgs *) void_args;
    auto mailbox = rsg::Mailbox::by_name(args->mbox);

    RSG_INFO("Sending '%s' on '%s'...", args->msg.c_str(), mailbox->get_cname());
    mailbox->put((void*) args->msg.data(), args->msg.size(), args->msg.size());

    RSG_INFO("I'm done. See you.");
    delete args;
}

struct ForwarderArgs { std::string mbox_in; std::string mbox_out; };
void forwarder(void * void_args)
{
    auto args = (ForwarderArgs *) void_args;

    auto in = rsg::Mailbox::by_name(args->mbox_in);
    auto out = rsg::Mailbox::by_name(args->mbox_out);

    RSG_INFO("Reading on '%s'...", in->get_cname());
    uint64_t bytes_read;
    auto msg = (char*) in->get(bytes_read);
    RSG_INFO("Got '%s'. Forwarding to '%s'...", msg, out->get_cname());
    out->put(msg, bytes_read, bytes_read);

    RSG_INFO("I'm done. See you.");
    delete msg;
    delete args;
}

static void receiver(void * void_args)
{
    auto mailbox_name = (std::string *) void_args;
    auto mailbox = rsg::Mailbox::by_name(*mailbox_name);

    RSG_INFO("Reading on '%s'...", mailbox->get_cname());
    auto msg1 = (char *) mailbox->get();
    auto msg2 = (char *) mailbox->get();
    auto msg3 = (char *) mailbox->get();
    RSG_INFO("Got '%s', '%s' and '%s'", msg1, msg2, msg3);

    delete msg1;
    delete msg2;
    delete msg3;

    RSG_INFO("I'm done. See you.");
    delete mailbox_name;
}

int main(int argc, char** argv)
{
    rsg::Actor::create("sender0", rsg::Host::by_name("Tremblay"), sender, (void*) new SenderArgs{"mitm", "PopPop!"});
    rsg::Actor::create("sender1", rsg::Host::by_name("Tremblay"), sender, (void*) new SenderArgs{"mb42", "GaBuZoMeu"});
    rsg::Actor::create("sender2", rsg::Host::by_name("Jupiter"), sender, (void*) new SenderArgs{"mb42", "GloubiBoulga"});
    rsg::Actor::create("forwarder", rsg::Host::by_name("Ginette"), forwarder, (void*) new ForwarderArgs{"mitm", "mb42"});
    rsg::Actor::create("receiver", rsg::Host::by_name("Fafard"), receiver, (void*) new std::string{"mb42"});

    return 0;
}
