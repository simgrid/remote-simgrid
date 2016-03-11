#include "rsg/host.hpp"
#include "rsg.pb.h"

using namespace simgrid;

XBT_LOG_EXTERNAL_CATEGORY(RSG);
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL_HOST, RSG,"RSG Host");


rsg::Host *rsg::Host::p_self = NULL;

rsg::Host::Host(const char *name) {
	p_name = std::string(name);
};

rsg::Host::Host(const std::string name) {
	p_name = std::string(name);
};

rsg::Host::~Host() {}

rsg::Host *rsg::Host::current() {
	if (p_self == NULL) {
    rsg::Request req;
    rsg::Answer ans;
  	req.set_type(rsg::CMD_HOST_CURRENT);
  	Engine::getInstance().sendRequest(req,ans);
		const char *name = ans.hostgetcurrent().hostname().c_str();
    p_self = new Host(name);
  	ans.Clear();
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}
	return p_self;
}
