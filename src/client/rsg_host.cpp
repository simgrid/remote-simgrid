#include "rsg/host.hpp"
#include "client/RsgClientEngine.hpp"
#include "rsg/services.hpp"

using namespace ::simgrid;

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL_HOST, RSG,"RSG Host");
XBT_LOG_EXTERNAL_CATEGORY(RSG);

using namespace simgrid;

rsg::Host::Host(const char *name, unsigned long int remoteAddr) {
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
};

rsg::Host::Host(const simgrid::xbt::string name, unsigned long int remoteAddr) {
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
};

rsg::Host::~Host() {}

rsg::Host &rsg::Host::current() {
	rsgHostCurrentResType res;
	
	ClientEngine& engine = ClientEngine::getInstance();
	engine.serviceClientFactory<RsgHostClient>("RsgHost").current(res);
	
  return *(new Host(res.name, res.addr)); // FIXME never deleted

}

rsg::Host &rsg::Host::by_name(std::string name) {
	ClientEngine& engine = ClientEngine::getInstance();
	unsigned long int addr = engine.serviceClientFactory<RsgHostClient>("RsgHost").by_name(name);
	if(addr == 0) {
			xbt_die("No such host: %s", name.c_str());
	}
	return *(new rsg::Host(name, addr));
}

double rsg::Host::speed() {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").speed(p_remoteAddr);
}


void rsg::Host::turnOn() {
	ClientEngine& engine = ClientEngine::getInstance();
	engine.serviceClientFactory<RsgHostClient>("RsgHost").turnOn(p_remoteAddr);
}

void rsg::Host::turnOff() {
	ClientEngine& engine = ClientEngine::getInstance();
	engine.serviceClientFactory<RsgHostClient>("RsgHost").turnOff(p_remoteAddr);
}

bool rsg::Host::isOn() {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").isOn(p_remoteAddr);
}

double rsg::Host::currentPowerPeak() {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").currentPowerPeak(p_remoteAddr);
}
double rsg::Host::powerPeakAt(int pstate_index) {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").powerPeakAt(p_remoteAddr, pstate_index);
}

int rsg::Host::pstatesCount() const {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").pstatesCount(p_remoteAddr);
}

void rsg::Host::setPstate(int pstate_index) {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").setPstate(p_remoteAddr, pstate_index);
}

int rsg::Host::pstate() {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").pstate(p_remoteAddr);
}

int rsg::Host::core_count() {
	ClientEngine& engine = ClientEngine::getInstance();
	return engine.serviceClientFactory<RsgHostClient>("RsgHost").core_count(p_remoteAddr);
}
