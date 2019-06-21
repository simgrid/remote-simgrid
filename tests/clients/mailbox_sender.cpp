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

    int i = 1337;
    mbox->put(&i, 4, 4);
    mbox->put(&i, 4, 16*1024*1024);

    std::string text = "Gi nathlam hí";
    mbox->put((void*)text.data(), text.size(), text.size());

    return 0;
}
