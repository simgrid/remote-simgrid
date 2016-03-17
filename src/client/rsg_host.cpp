#include "rsg/host.hpp"
#include "rsg.pb.h"

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL_HOST, RSG,"RSG Host");
XBT_LOG_EXTERNAL_CATEGORY(RSG);

using namespace simgrid;
boost::unordered_map <std::string, rsg::Host *> *rsg::Host::hosts = new boost::unordered_map<std::string, rsg::Host*> ();

rsg::Host *rsg::Host::p_self = NULL;

rsg::Host::Host(const char *name, unsigned long int remoteAddr) {
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
	hosts->insert({name, this});
};

rsg::Host::Host(const simgrid::xbt::string name, unsigned long int remoteAddr) {
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
	hosts->insert({name, this});
};

rsg::Host::~Host() {}

rsg::Host *rsg::Host::current() {
	if (p_self == NULL) {
    rsg::Request req;
    rsg::Answer ans;
  	req.set_type(rsg::CMD_HOST_CURRENT);
  	Engine::getInstance().sendRequest(req,ans);
		const char *name = ans.hostgetcurrent().hostname().c_str();
		unsigned long int remoteAddr = ans.hostgetcurrent().remoteaddr();
    p_self = new Host(name, remoteAddr);
  	ans.Clear();
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}
	return p_self;
}

rsg::Host *rsg::Host::by_name(std::string name) {
	rsg::Host * res;
	try {
		res = hosts->at(name);
	} catch (std::out_of_range& e) {
		rsg::Request req;
		rsg::Answer ans;
		req.set_type(rsg::CMD_HOST_GETBYNAME);
		req.mutable_hostgetbyname()->set_name(name);

		rsg::Engine::getInstance().sendRequest(req, ans);

		std::string name = ans.hostgetbyname().hostname();
		unsigned long int remoteAddr = ans.hostgetbyname().remoteaddr();
		ans.Clear();
		res = new rsg::Host(name, remoteAddr);
	}
	return res;
}

double rsg::Host::speed() {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_HOST_GETSPEED);
	req.mutable_hostgetspeed()->set_host(this->p_remoteAddr);
	Engine::getInstance().sendRequest(req, ans);
	double speed = ans.hostgetspeed().speed();
	ans.Clear();
	return speed;
}

void rsg::Host::shutdown() {
	rsg::Host::hosts->clear();
	delete rsg::Host::hosts;
}
