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

    int * i;
    auto comm = mbox->get_async((void**) &i);
    comm->wait();
    printf("Received: %d\n", *i);
    delete[] i;

    comm = mbox->get_async((void**) &i);
    comm->wait();
    printf("Received: %d\n", *i);
    delete[] i;

    char * text;
    comm = mbox->get_async((void**) &text);
    comm->wait();
    printf("Received: '%s'", text);
    delete[] text;

    return 0;
}
