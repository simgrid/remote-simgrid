#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main()
{
    auto actor = rsg::Actor::self();
    printf("My actor is (pid=%d, name='%s'), located on Host(name='%s')\n",
        actor->get_pid(), actor->get_name().c_str(), actor->get_host()->get_name().c_str());

    auto mbox = rsg::Mailbox::by_name("testbox");
    printf("mbox '%s' empty? %d\n", mbox->get_name().c_str(), mbox->empty());
    printf("mbox '%s' listen? %d\n", mbox->get_cname(), mbox->listen());
    printf("mbox '%s' ready? %d\n", mbox->get_cname(), mbox->ready());

    int * i = (int *) mbox->get();
    printf("Received: %d\n", *i);
    delete i;

    uint64_t bytes_read;
    i = (int *) mbox->get(bytes_read);
    printf("Received: %d (read %lu bytes)\n", *i, bytes_read);
    delete i;

    char * text = (char *) mbox->get();
    printf("Received: '%s'", text);
    delete text;

    return 0;
}
