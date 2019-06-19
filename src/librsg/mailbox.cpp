#include "connection.hpp"
#include "mailbox.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::Mailbox::Mailbox(const std::string & name) : _name(name)
{
}

std::shared_ptr<rsg::Mailbox> rsg::Mailbox::by_name(const std::string & name)
{
    return std::shared_ptr<rsg::Mailbox>(new rsg::Mailbox(name));
}

bool rsg::Mailbox::empty()
{
    rsg::pb::Decision decision;
    auto mbox = new rsg::pb::Mailbox();
    mbox->set_name(_name);
    decision.set_allocated_mailboxempty(mbox);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    return ack.success();
}

bool rsg::Mailbox::listen()
{
    rsg::pb::Decision decision;
    auto mbox = new rsg::pb::Mailbox();
    mbox->set_name(_name);
    decision.set_allocated_mailboxlisten(mbox);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    return ack.success();
}

bool rsg::Mailbox::ready()
{
    rsg::pb::Decision decision;
    auto mbox = new rsg::pb::Mailbox();
    mbox->set_name(_name);
    decision.set_allocated_mailboxready(mbox);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    return ack.success();
}

void rsg::Mailbox::put(void * data, uint64_t size, uint64_t simulated_size)
{
    rsg::pb::Decision decision;
    auto mbox = new rsg::pb::Mailbox();
    mbox->set_name(_name);
    auto mbox_put = new rsg::pb::Decision_MailboxPut();
    mbox_put->set_allocated_mailbox(mbox);
    mbox_put->set_data(data, size);
    mbox_put->set_simulatedsize(simulated_size);
    decision.set_allocated_mailboxput(mbox_put);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not put on mailbox='%s'", _name.c_str());
}

void * rsg::Mailbox::get()
{
    uint64_t bytes_read;
    return get(bytes_read);
}

void * rsg::Mailbox::get(uint64_t & size)
{
    rsg::pb::Decision decision;
    auto mbox = new rsg::pb::Mailbox();
    mbox->set_name(_name);
    decision.set_allocated_mailboxget(mbox);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not get on mailbox='%s'", _name.c_str());

    const std::string & data = ack.mailboxget();
    size = data.size();

    if (data.size() == 0)
        return nullptr;

    uint8_t * ret = new uint8_t[data.size()];
    memcpy(ret, data.data(), data.size());
    return ret;
}

std::string rsg::Mailbox::get_name() const
{
    return _name;
}

const char * rsg::Mailbox::get_cname() const
{
    return _name.c_str();
}
