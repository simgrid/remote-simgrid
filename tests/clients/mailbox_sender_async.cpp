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

    int i = 1337;
    auto comm = mbox->put_async(&i, 4, 4);
    comm->wait();

    comm = mbox->put_async(&i, 4, 16*1024*1024);
    comm->wait();

    std::string text = "Gi nathlam hí";
    comm = mbox->put_async((void*)text.data(), text.size(), text.size());
    comm->wait();

    return 0;
}
