#include "host.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::Host::Host(const std::string & name) : _name(name)
{
}

rsg::HostPtr rsg::Host::by_name(const std::string & name)
{
    auto host = by_name_or_null(name);
    RSG_ENFORCE(host != nullptr, "There is no host named '%s'", name.c_str());
    return host;
}

rsg::HostPtr rsg::Host::by_name_or_null(const std::string & name)
{
    rsg::pb::Decision decision;
    decision.set_hostbynameornull(name);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    if (!ack.success())
        return nullptr;
    return rsg::HostPtr(new rsg::Host(name));
}

std::string rsg::Host::get_name() const
{
    return _name;
}

const char * rsg::Host::get_cname() const
{
    return _name.c_str();
}
