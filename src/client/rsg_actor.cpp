/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "../common.hpp"
#include "../rsg/services.hpp"

#include "rsg/engine.hpp"
#include "rsg/actor.hpp"
#include "RsgClient.hpp"
#include "multiThreadedSingletonFactory.hpp"


#include <sys/wait.h> 
#include <string>
#include <iostream>
#include <unordered_set>

using namespace ::simgrid;

std::unordered_set<size_t> deconnected_threads;

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor::Actor(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr), pHost(NULL) {
}


void rsg::Actor::kill() { 
    
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").kill(this->p_remoteAddr);
    
}

void rsg::Actor::kill(int pid) {
    
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").killPid(pid);
    
}

void rsg::Actor::join(void) {
    
    MultiThreadedSingletonFactory &factory = MultiThreadedSingletonFactory::getInstance();
    Client& engine = factory.getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").join(this->p_remoteAddr);
    
}

void rsg::this_actor::quit(void) {
    std::hash<std::thread::id> hasher;
    size_t hashed_thread_id = hasher(std::this_thread::get_id());
    const bool is_in = deconnected_threads.find(hashed_thread_id) != deconnected_threads.end();
    if(!is_in) {
        //in case if the user call quit a second time on an actor we won't try to deconnect a second time otherwise RSG will block
        MultiThreadedSingletonFactory &factory = MultiThreadedSingletonFactory::getInstance();
        Client& engine = factory.getClient(std::this_thread::get_id());
        engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
        factory.clearClient(std::this_thread::get_id());
        deconnected_threads.insert(hashed_thread_id);
    }
}

void rsg::this_actor::sleep(const double duration) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").sleep(duration);
}

void rsg::this_actor::execute(const double flops) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").execute(flops);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content, size_t dataSize) {
    rsg::this_actor::send(mailbox, content, dataSize, dataSize);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content,size_t dataSize, size_t simulatedSize) {
    std::string strContent(content, dataSize);
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::this_actor::recv(Mailbox &mailbox) {
    std::string res;
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").recv(res, mailbox.getRemote());
    char *content = (char*) malloc(sizeof(char*) * res.length());
    memcpy(content, res.data(), res.length());
    return content;
}

char* rsg::Actor::getName() {
    std::string res;
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").getName(res, this->p_remoteAddr);
    char* res_cstr = (char*) malloc( (sizeof(char*) * res.length()) + 1);
    
    strcpy(res_cstr, res.c_str());
    return res_cstr;
}

rsg::Host* rsg::Actor::getHost() {
    rsgHostCurrentResType res;
    if(pHost == NULL) {
        Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
        engine.serviceClientFactory<RsgActorClient>("RsgActor").getHost(res, this->p_remoteAddr);
        pHost = new Host(res.name, res.addr);
    }
    return pHost;
}

int rsg::Actor::getPid() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return  engine.serviceClientFactory<RsgActorClient>("RsgActor").getPid(this->p_remoteAddr);
}

int rsg::Actor::getPPid() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return  engine.serviceClientFactory<RsgActorClient>("RsgActor").getPPid(this->p_remoteAddr);
}

void rsg::Actor::setAutoRestart(bool autorestart) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").setAutoRestart(this->p_remoteAddr, autorestart);
}

void rsg::Actor::setKillTime(double time){
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").setKillTime(this->p_remoteAddr, time);
}

double rsg::Actor::getKillTime() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.serviceClientFactory<RsgActorClient>("RsgActor").getKillTime(this->p_remoteAddr);
}

void rsg::Actor::killAll() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").killAll();
}

rsg::Actor* rsg::Actor::forPid(int pid) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    int64_t addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").forPid(pid);
    if(addr == -1)
        return 0;
    else
        return new Actor(addr);
}

void actorRunner(std::function<int(void *)> code, int port, void *data ) {
    MultiThreadedSingletonFactory::getInstance().getClientOrCreate(std::this_thread::get_id(), port);
    int pid = rsg::this_actor::getPid();

    // debug_spawn_client("Actor [%d] running", pid);
    try {
        code(data);
    } catch(apache::thrift::TApplicationException &ex) {
        std::cerr<< "apache::thrift::TApplicationException in thread : " << ex.what() << std::endl;
    } catch(apache::thrift::transport::TTransportException &ex) {
        //std::cerr<< "apache::thrift::transport::TTransportException in thread : " << ex.what() << std::endl;
        // this exeption occure when the process have been killed on the server by another process.
        MultiThreadedSingletonFactory &factory = MultiThreadedSingletonFactory::getInstance();
        factory.clearClient(std::this_thread::get_id());
    }

    rsg::this_actor::quit();
    debug_spawn_client("Actor [%d] quit", pid);
    
}

rsg::Actor *rsg::Actor::createActor(std::string name, rsg::HostPtr host, std::function<int(void *)> code, void *data) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());    
    rsgServerRemoteAddrAndPort params;
    engine.serviceClientFactory<RsgActorClient>("RsgActor").createActorPrepare(params);
    
    rsg::Actor *self = rsg::Actor::self();
    char *cstr_name = self->getName();
    delete self;

    std::thread *nActor = new std::thread(actorRunner, code, params.port, data);         
    MultiThreadedSingletonFactory::getInstance().registerNewThread(nActor);
    unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").createActor(params.addr, params.port ,name, host->p_remoteAddr, 10);
    rsg::Actor *act = new Actor(addr);
    debug_spawn_client("Actor [%d]%s creates thread [%d]", this_actor::getPid(), cstr_name, act->getPid());
    free(cstr_name);
    return act;
}

rsg::Actor *rsg::Actor::self() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").selfAddr();
    rsg::Actor *act = new Actor(addr);
    return act;
}

int rsg::this_actor::getPid() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.serviceClientFactory<RsgActorClient>("RsgActor").this_actorGetPid();
}

int rsg::this_actor::getPPid() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.serviceClientFactory<RsgActorClient>("RsgActor").this_actorGetPPid();
}

void actorForkRunner(std::promise<int> &child_pid, int64_t client, int64_t addr, int port, int64_t hostAddr, std::string childName) {
    Client *engine = (Client*)client;
    unsigned long long newActorAddr = engine->serviceClientFactory<RsgActorClient>("RsgActor").createActor(addr, port, childName, hostAddr, 10);
    int newPid = engine->serviceClientFactory<RsgActorClient>("RsgActor").getPid(newActorAddr);
    child_pid.set_value(newPid);
}

int rsg::this_actor::fork(std::string childName) {
    Client  *engine = &MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    rsgServerRemoteAddrAndPort params;
    
    long int hostAddr = Host::current()->p_remoteAddr;

    rsg::Actor *self = rsg::Actor::self();
    char *c_name = self->getName();
    std::string name(c_name);
    free(c_name);
    delete self;

    engine->serviceClientFactory<RsgActorClient>("RsgActor").createActorPrepare(params);
    MultiThreadedSingletonFactory::getInstance().flushAll();
    
    pid_t pid = ::fork();
    if(0 == pid) { // Child
        //FIXME There is a memory leak in fork because we cannot clear call "reset" on all client before clear them. 
        // It willl lead to a leak of all created client. But if we clear all client the program will block (with fork_from_spwaned_actor test for example).
        // This happends becaus when we delete a client, it closes its connection. 
        // rsg::setKeepAliveOnNextClientDisconnect(true);
        MultiThreadedSingletonFactory::getInstance().clearAll(true);
        MultiThreadedSingletonFactory::getInstance().getClientOrCreate(std::this_thread::get_id(), params.port);
        return 0;
    }
    std::promise<int> child_pid;
    std::future<int> future = child_pid.get_future();
    
    std::thread nActor(actorForkRunner, std::ref(child_pid), (int64_t) engine, params.addr, params.port, hostAddr, childName);
    nActor.join();
    future.wait();
    int res = future.get();
    debug_spawn_client(" Actor [%d]%s  creates fork [%d]%s", this_actor::getPid(), name.c_str(), res, childName.c_str());
    return res;
}

bool rsg::Actor::valid() const { 
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.serviceClientFactory<RsgActorClient>("RsgActor").isValideActor(this->p_remoteAddr);
}

rsg::Actor::~Actor() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").deleteActor(this->p_remoteAddr);
}
