#include "rsg/host.hpp"
#include "client/RsgClientEngine.hpp"
#include "rsg/services.hpp"

using namespace ::simgrid;

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
	return pHostService->speed(p_remoteAddr);
}

void rsg::Host::initNetworkService() {
	ClientEngine& engine = ClientEngine::getInstance();
	pHostService = boost::shared_ptr<RsgHostClient>(engine.serviceClientFactory<RsgHostClient>("RsgHost"));
}

void rsg::Host::turnOn() {
	pHostService->turnOn(p_remoteAddr);
}

void rsg::Host::turnOff() {
	pHostService->turnOff(p_remoteAddr);
}

bool rsg::Host::isOn() {
	return pHostService->isOn(p_remoteAddr);
}

double rsg::Host::currentPowerPeak() {
  return pHostService->currentPowerPeak(p_remoteAddr);
}
double rsg::Host::powerPeakAt(int pstate_index) {
  return pHostService->powerPeakAt(p_remoteAddr, pstate_index);
}

int rsg::Host::pstatesCount() const {
  return pHostService->pstatesCount(p_remoteAddr);
}

void rsg::Host::setPstate(int pstate_index) {
  pHostService->setPstate(p_remoteAddr, pstate_index);
}

int rsg::Host::pstate() {
  return pHostService->pstate(p_remoteAddr);
}

int rsg::Host::core_count() {
  return pHostService->core_count(p_remoteAddr);
}
