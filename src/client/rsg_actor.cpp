/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "RsgMsg.hpp"
#include "client/RsgClient.hpp"
#include "client/multiThreadedSingletonFactory.hpp"

#include <sys/wait.h> 
#include <string>
#include <iostream>

using namespace ::simgrid;


XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor::Actor(unsigned long int remoteAddr, int pid) : p_remoteAddr(remoteAddr), pHost(NULL), pThreadId(pid) {
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
    
    MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
    Client& engine = factory.getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").join(this->p_remoteAddr);
    
}

void rsg::this_actor::quit(void) {
    
    MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
    Client& engine = factory.getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
    factory.clearClient(std::this_thread::get_id());
    
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
    unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").forPid(pid);
    if(addr == 0)
        return 0;
    else
        return new Actor(addr, pid);
}

void actorRunner(std::function<int(void *)> code, int port, void *data ) {
    MultiThreadedSingletonFactory::getInstance().getClientOrCreate(std::this_thread::get_id(), port);
    try {
        code(data);
    } catch(apache::thrift::TApplicationException &ex) {
        std::cerr<< "apache::thrift::TApplicationException in thread : " << ex.what() << std::endl;
    } catch(apache::thrift::transport::TTransportException &ex) {
        //std::cerr<< "apache::thrift::transport::TTransportException in thread : " << ex.what() << std::endl;
        // this exeption occure when the process have been killed on the server by another process.
        MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
        factory.clearClient(std::this_thread::get_id());
    }
}

rsg::Actor *rsg::Actor::createActor(std::string name, rsg::Host host, std::function<int(void *)> code, void *data) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    
    rsgServerRemoteAddrAndPort params;
    engine.serviceClientFactory<RsgActorClient>("RsgActor").createActorPrepare(params);
    
    std::thread *nActor = new std::thread(actorRunner, code, params.port, data);     
    MultiThreadedSingletonFactory::getInstance().registerNewThread(nActor);
    unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").createActor(params.addr, params.port ,name, host.p_remoteAddr, 10);
    int newPid = engine.serviceClientFactory<RsgActorClient>("RsgActor").getPid(addr);
    rsg::Actor *act = new Actor(addr, newPid);
    return act;
}

int rsg::this_actor::getPid() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.serviceClientFactory<RsgActorClient>("RsgActor").this_actorGetPid();
}

void actorForkRunner(std::promise<int> &child_pid, int64_t client, int64_t addr, int port, int64_t hostAddr) {
    Client *engine = (Client*)client;
    unsigned long long newActorAddr = engine->serviceClientFactory<RsgActorClient>("RsgActor").createActor(addr, port ,"forked", hostAddr, 10);
    int newPid = engine->serviceClientFactory<RsgActorClient>("RsgActor").getPid(newActorAddr);
    child_pid.set_value(newPid);
}

int rsg::this_actor::fork() {
    Client  *engine = &MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    
    rsgServerRemoteAddrAndPort params;
    engine->serviceClientFactory<RsgActorClient>("RsgActor").createActorPrepare(params);
    
    pid_t pid = ::fork();
    if(0 == pid) { // Child
        MultiThreadedSingletonFactory::getInstance().clearAll(false);
        MultiThreadedSingletonFactory::getInstance().getClientOrCreate(std::this_thread::get_id(), params.port);
        return 0;
    }
    
    std::promise<int> child_pid;
    std::future<int> future = child_pid.get_future();

    std::thread nActor(actorForkRunner, std::ref(child_pid), (int64_t) engine, params.addr, params.port, Host::current().p_remoteAddr);
    nActor.join();
    future.wait();
    int res = future.get(); 
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
