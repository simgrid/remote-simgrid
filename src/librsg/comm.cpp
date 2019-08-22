#include "connection.hpp"
#include "comm.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::Comm::Comm(uint64_t remote_address, void ** destination_buffer) :
    _remote_address(remote_address), _destination_buffer(destination_buffer)
{
}

rsg::Comm::~Comm()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::Comm();
    comm->set_address(_remote_address);
    decision.set_allocated_commrefcountdecrease(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

void rsg::Comm::start()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::Comm();
    comm->set_address(_remote_address);
    decision.set_allocated_commstart(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not start Comm(addr=%lu)", _remote_address);
}

void rsg::Comm::wait()
{
    return wait_for(-1);
}

void rsg::Comm::wait_for(double timeout)
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::Comm();
    comm->set_address(_remote_address);
    auto comm_wait_for = new rsg::pb::Decision_CommWaitFor();
    comm_wait_for->set_allocated_comm(comm);
    comm_wait_for->set_timeout(timeout);
    decision.set_allocated_commwaitfor(comm_wait_for);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "wait_for(%g) failed on Comm(addr=%lu)", timeout, _remote_address);
    RSG_ENFORCE(!ack.commwaitfor().timeoutreached(), "wait_for(%g) on Comm(addr=%lu): Timeout reached", timeout, _remote_address); // TODO: proper exception

    if (_destination_buffer != nullptr)
    {
        auto & data = ack.commwaitfor().data();
        RSG_ASSERT(data.size() > 0, "Received CommWaitFor ack does not contain data while _destination_buffer is not null");
        // Create a buffer for the user.
        auto buffer = new uint8_t[data.size()];
        memcpy(buffer, data.data(), data.size());
        *_destination_buffer = buffer;
    }
}

void rsg::Comm::cancel()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::Comm();
    comm->set_address(_remote_address);
    decision.set_allocated_commcancel(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not cancel Comm(addr=%lu)", _remote_address);
}

bool rsg::Comm::test()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::Comm();
    comm->set_address(_remote_address);
    decision.set_allocated_commtest(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not test Comm(addr=%lu)", _remote_address);

    return ack.commtest();
}

int rsg::Comm::wait_any_for(const std::vector<rsg::CommPtr> & comms, double timeout)
{
    rsg::pb::Decision decision;
    auto wait_any_for = new rsg::pb::Decision_CommWaitAnyFor();
    wait_any_for->set_timeout(timeout);

    auto buf = wait_any_for->mutable_comms();
    buf->Reserve(comms.size());
    for (const auto & comm : comms)
    {
        auto c = new rsg::pb::Comm();
        c->set_address(comm->_remote_address);
        buf->AddAllocated(c);
    }
    decision.set_allocated_commwaitanyfor(wait_any_for);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Comm::wait_any_for failed");

    int finished_index = ack.commwaitanyfor().finishedcommindex();
    if (finished_index >= 0) // timeout has NOT been reached
    {
        auto & comm = comms.at(finished_index);
        if (comm->_destination_buffer != nullptr)
        {
            auto & data = ack.commwaitanyfor().data();
            RSG_ASSERT(data.size() > 0, "Received CommWaitAnyFor ack does not contain data while _destination_buffer is not null");
            // Create a buffer for the user.
            auto buffer = new uint8_t[data.size()];
            memcpy(buffer, data.data(), data.size());
            *comm->_destination_buffer = buffer;
        }
    }

    return finished_index;
}

int rsg::Comm::wait_any(const std::vector<rsg::CommPtr> & comms)
{
    return wait_any_for(comms, -1);
}
