#include "rsg/host.hpp"
#include "client/RsgClientEngine.hpp"
#include "rsg/RsgServiceImpl.h"


XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL_HOST, RSG,"RSG Host");
XBT_LOG_EXTERNAL_CATEGORY(RSG);

using namespace simgrid;
boost::unordered_map <std::string, rsg::Host *> *rsg::Host::hosts = new boost::unordered_map<std::string, rsg::Host*> ();
boost::shared_ptr<RsgHostClient> rsg::Host::pHostService(NULL);

rsg::Host *rsg::Host::pSelf = NULL;

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

rsg::Host &rsg::Host::current() {
	if (pSelf == NULL) {
		if(!pHostService) initNetworkService();
		rsgHostCurrentResType res;
		pHostService->current(res);
    pSelf = new Host(res.name, res.addr);
	}
	return *pSelf;
}

rsg::Host &rsg::Host::by_name(std::string name) {
	if(!pHostService) initNetworkService();
	try {
		return *Host::hosts->at(name);
	} catch (std::out_of_range& e) {
		unsigned long int addr = pHostService->by_name(name);
		if(addr == 0) {
 			xbt_die("No such host: %s", name.c_str());
		}
		return *(new rsg::Host(name, addr));
	}
}

double rsg::Host::speed() {
		return 0;
}

void rsg::Host::initNetworkService() {
	ClientEngine& engine = ClientEngine::getInstance();
	pHostService = boost::shared_ptr<RsgHostClient>(engine.serviceClientFactory<RsgHostClient>("RsgHost"));
}
