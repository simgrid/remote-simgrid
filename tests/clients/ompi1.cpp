#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

using namespace std;

enum CommandType
{
    SEND,
    RECV,
    TERMINATE,
};

struct BackgroundParameters
{
    rsg::MailboxPtr background_mb;
    rsg::MailboxPtr callback_mb;
    rsg::MailboxPtr my_mb;
    rsg::MailboxPtr other_mb;
};

int data = -1;
int oth_data = -1;

void background(void * void_arg)
{
    auto arg = (BackgroundParameters *) void_arg;

    int * command = nullptr;
    auto command_comm = arg->background_mb->get_async((void**) &command);
    std::vector<rsg::CommPtr> pending_comms = {command_comm};

    for (bool termination_requested = false; !termination_requested; ) {
        printf("wait_any on %zu comms\n", pending_comms.size());
        int finished_index = rsg::Comm::wait_any(pending_comms);
        auto finished_comm = pending_comms.at(finished_index);

        if (finished_comm == command_comm) {
            if (*command == TERMINATE) {
                printf("TERMINATE command received\n");
                termination_requested = true;
            }
            else if (*command == SEND) {
                printf("SEND command received\n");
                auto send_comm = arg->other_mb->put_async(&data, 4, 4);
                pending_comms.push_back(send_comm);
            }
            else {
                printf("RECV command received\n");
                auto recv_comm = arg->my_mb->get_async((void**) &oth_data);
                pending_comms.push_back(recv_comm);
            }

            if (*command != TERMINATE) {
                command_comm = arg->background_mb->get_async((void**) &command);
                pending_comms.push_back(command_comm);
            }
        } else {
            printf("Non-command comm has finished\n");
            arg->callback_mb->put(&data, 4, 4);
        }

        pending_comms.erase(pending_comms.begin() + finished_index);
    }
}

int main(int argc, char * argv[])
{
    const char usage[] = "Usage: %s <rank>\n";
    if (argc != 2) {
        printf(usage, argv[0]);
        return 1;
    }

    string rank = string(argv[1]);
    if (rank != "0" && rank != "1") {
        printf("rank='%s' is not 0 nor 1, aborting.\n", rank.c_str());
        return 1;
    }
    data = stoi(rank);
    string other_rank = "1";
    if (rank == "1")
        other_rank = "0";

    BackgroundParameters p;
    p.callback_mb = rsg::Mailbox::by_name(string("cb_") + rank);
    p.background_mb = rsg::Mailbox::by_name(string("bg_") + rank);
    p.my_mb = rsg::Mailbox::by_name(rank);
    p.other_mb = rsg::Mailbox::by_name(other_rank);

    printf("create background actor\n");
    rsg::Actor::create(p.background_mb->get_name(), rsg::Actor::self()->get_host(), background, (void*)&p);

    printf("tell background actor to do operation\n");
    p.background_mb->put(&data, 4, 4);

    printf("wait until operation is done\n");
    p.callback_mb->get();

    printf("terminate background actor\n");
    int terminate = TERMINATE;
    p.background_mb->put(&terminate, 4, 4);

    printf("exit\n");
    return 0;
}
