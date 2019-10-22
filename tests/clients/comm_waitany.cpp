#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main()
{
    auto mbox = rsg::Mailbox::by_name("mailbox");

    int send_data[2] = {42, 51};
    int * recv_data1;
    int * recv_data2;
    RSG_INFO("Sending and receiving %d and %d asynchronously", send_data[0], send_data[1]);

    auto put1 = mbox->put_async(send_data, 4, 4);
    auto put2 = mbox->put_async(send_data + 1, 4, 32*1024*1024);
    auto get1 = mbox->get_async((void**) &recv_data1);
    auto get2 = mbox->get_async((void**) &recv_data2);
    std::vector<rsg::CommPtr> pending = {put1, put2, get1, get2};
    RSG_INFO("All comms have started.");

    RSG_INFO("Waiting for a first comm to finish via wait_any");
    int finished_index = rsg::Comm::wait_any(pending);
    RSG_INFO("A comm has finished (index=%d)", finished_index);
    pending.erase(pending.begin() + finished_index);

    const double timeout = 1;
    RSG_INFO("Waiting for the other pending comms to finish via wait_any_for (timeout=%g)", timeout);

    while (!pending.empty()) {
        finished_index = rsg::Comm::wait_any_for(pending, 0.5);
        if (finished_index < 0)
            RSG_INFO("Timeout reached (got index=%d)", finished_index);
        else {
            RSG_INFO("A comm has finished (index=%d, #comms=%zu)", finished_index, pending.size());
            pending.erase(pending.begin() + finished_index);
        }
    }

    RSG_INFO("All comms have finished. Received %d and %d", *recv_data1, *recv_data2);
    delete[] recv_data1;
    delete[] recv_data2;

    return 0;
}
