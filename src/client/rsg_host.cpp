#include "rsg/host.hpp"
#include "client/RsgClient.hpp"
#include "rsg/services.hpp"
#include "client/multiThreadedSingletonFactory.hpp"
#include "RsgMsg.hpp"
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
	
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	engine.serviceClientFactory<RsgHostClient>("RsgHost").current(res);
	
  return *(new Host(res.name, res.addr)); // FIXME never deleted

}

rsg::Host &rsg::Host::by_name(std::string name) {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	unsigned long int addr = engine.serviceClientFactory<RsgHostClient>("RsgHost").by_name(name);
	if(addr == 0) {
			xbt_die("No such host: %s", name.c_str());
	}
	return *(new rsg::Host(name, addr));
}

double rsg::Host::speed() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").speed(p_remoteAddr);
}


void rsg::Host::turnOn() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	engine.serviceClientFactory<RsgHostClient>("RsgHost").turnOn(p_remoteAddr);
}

void rsg::Host::turnOff() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	engine.serviceClientFactory<RsgHostClient>("RsgHost").turnOff(p_remoteAddr);
}

bool rsg::Host::isOn() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").isOn(p_remoteAddr);
}

double rsg::Host::currentPowerPeak() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").currentPowerPeak(p_remoteAddr);
}
double rsg::Host::powerPeakAt(int pstate_index) {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").powerPeakAt(p_remoteAddr, pstate_index);
}

int rsg::Host::pstatesCount() const {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").pstatesCount(p_remoteAddr);
}

void rsg::Host::setPstate(int pstate_index) {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").setPstate(p_remoteAddr, pstate_index);
}

int rsg::Host::pstate() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").pstate(p_remoteAddr);
}

int rsg::Host::core_count() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());

	return engine.serviceClientFactory<RsgHostClient>("RsgHost").core_count(p_remoteAddr);
}
