#include "rsg/host.hpp"
#include "../rsg/services.hpp"

#include "../common.hpp"
#include "RsgClient.hpp"

#include <iostream>
using namespace ::simgrid;

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL_HOST, RSG,"RSG Host");
XBT_LOG_EXTERNAL_CATEGORY(RSG);

using namespace simgrid;

rsg::Host::Host(const char *name, unsigned long int remoteAddr) {
    debug_client_print("Creating Host1 %s", name);
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
};

rsg::Host::Host(const simgrid::xbt::string name, unsigned long int remoteAddr) {
    debug_client_print("Creating Host2 %s", name.c_str());
	name_ = simgrid::xbt::string(name);
	p_remoteAddr = remoteAddr;
};

rsg::Host::~Host() {
}

rsg::HostPtr rsg::Host::current() {
        debug_client_print("COUCOU");
        
	rsgHostCurrentResType res;
        debug_client_print("COUCOU %p", client->host);
	
	client->host->current(res);
        
        debug_client_print("COUCOU %s // %lu", res.name.c_str(), res.addr);
	
	return HostPtr(new Host(res.name, res.addr)); 
}

rsg::HostPtr rsg::Host::by_name(std::string name) {
	unsigned long int addr = client->host->by_name(name);
	if(addr == 0) {
		xbt_die("No such host: %s", name.c_str());
	}
	return HostPtr(new rsg::Host(name, addr));
}

double rsg::Host::speed() {
	return client->host->speed(p_remoteAddr);
}


void rsg::Host::turnOn() {
	client->host->turnOn(p_remoteAddr);
}

void rsg::Host::turnOff() {
	client->host->turnOff(p_remoteAddr);
}

bool rsg::Host::isOn() {
	return client->host->isOn(p_remoteAddr);
}

double rsg::Host::currentPowerPeak() {
	return client->host->currentPowerPeak(p_remoteAddr);
}
double rsg::Host::powerPeakAt(int pstate_index) {
	return client->host->powerPeakAt(p_remoteAddr, pstate_index);
}

int rsg::Host::pstatesCount() const {
	return client->host->pstatesCount(p_remoteAddr);
}

void rsg::Host::setPstate(int pstate_index) {
	return client->host->setPstate(p_remoteAddr, pstate_index);
}

int rsg::Host::pstate() {
	return client->host->pstate(p_remoteAddr);
}

int rsg::Host::coreCount() {
	return client->host->coreCount(p_remoteAddr);
}

/** Retrieve the property value (or nullptr if not set) */
char* rsg::Host::property(const char*key) {
	std::string res; 
	char *res_cstr = NULL;
	
	client->host->getProperty(res, p_remoteAddr, std::string(key).c_str());
	if(res.size() == 0) {
		return res_cstr;
	}
	res_cstr = new char[res.size() + 1];
	strcpy(res_cstr, res.c_str());
	return res_cstr;
}

void rsg::Host::setProperty(const char*key, const char *value){
	client->host->setProperty(p_remoteAddr, std::string(key), std::string(value));
}
