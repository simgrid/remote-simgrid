#include "connection.hpp"
#include "condition_variable.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::ConditionVariable::ConditionVariable(uint64_t remote_address) :
    _remote_address(remote_address), _destination_buffer(destination_buffer)
{
}

rsg::ConditionVariable::~ConditionVariable()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::ConditionVariable();
    comm->set_address(_remote_address);
    decision.set_allocated_commrefcountdecrease(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}
