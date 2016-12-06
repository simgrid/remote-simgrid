/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "../common.hpp"
#include "../rsg/services.hpp"

#include "rsg/engine.hpp"
#include "rsg/actor.hpp"
#include "RsgClient.hpp"


#include <sys/wait.h> 
#include <string>
#include <iostream>
#include <unordered_set>

using namespace ::simgrid;

//For Developpers: if you add a global variable, check the fork methods and add the appropriate code.

//TODO: David: is this struct realy useful ?
std::unordered_set<size_t> deconnected_threads;

std::unordered_set<std::thread*> child_threads;


XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor::Actor(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr), pHost(NULL) {
}

void rsg::Actor::kill() {
    client->actor->kill(this->p_remoteAddr);
}

void rsg::Actor::kill(int pid) {
    client->actor->killPid(pid);
}

void rsg::Actor::join(void) {
    client->actor->join(this->p_remoteAddr);
}

void rsg::this_actor::quit(void) {
   /* std::hash<std::thread::id> hasher;
    size_t hashed_thread_id = hasher(std::this_thread::get_id());
    const bool is_in = deconnected_threads.find(hashed_thread_id) != deconnected_threads.end();
    if(!is_in) {
        //in case if the user call quit a second time on an actor we won't try to deconnect a second time otherwise RSG will block*/
   debug_spawn_client("quitting %s %p", client->networkName_.c_str(), client);
        client->actor->close();
//         deconnected_threads.insert(hashed_thread_id);
    delete client;
    client = 0;
   debug_spawn_client("quitted !! %p", client);
  /* }*/
}

void rsg::this_actor::sleep(const double duration) {
    client->actor->sleep(duration);
}

void rsg::this_actor::execute(const double flops) {
    client->actor->execute(flops);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content, size_t dataSize) {
    rsg::this_actor::send(mailbox, content, dataSize, dataSize);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content,size_t dataSize, size_t simulatedSize) {
    std::string strContent(content, dataSize);
    client->actor->send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::this_actor::recv(Mailbox &mailbox) {
    std::string res;
    client->actor->recv(res, mailbox.getRemote());
    char *content = (char*) malloc(sizeof(char*) * res.length());
    memcpy(content, res.data(), res.length());
    return content;
}

char* rsg::Actor::getName() {
    std::string res;
    
    client->actor->getName(res, this->p_remoteAddr);
    char* res_cstr = (char*) malloc( (sizeof(char*) * res.length()) + 1);
    
    strcpy(res_cstr, res.c_str());
    return res_cstr;
}

rsg::Host* rsg::Actor::getHost() {
    rsgHostCurrentResType res;
    if(pHost == NULL) {
        
        client->actor->getHost(res, this->p_remoteAddr);
        pHost = new Host(res.name, res.addr);
    }
    return pHost;
}

int rsg::Actor::getPid() {
    return  client->actor->getPid(this->p_remoteAddr);
}

int rsg::Actor::getPPid() {
    return  client->actor->getPPid(this->p_remoteAddr);
}

void rsg::Actor::setAutoRestart(bool autorestart) {
    client->actor->setAutoRestart(this->p_remoteAddr, autorestart);
}

void rsg::Actor::setKillTime(double time){
    client->actor->setKillTime(this->p_remoteAddr, time);
}

double rsg::Actor::getKillTime() {
    return client->actor->getKillTime(this->p_remoteAddr);
}

void rsg::Actor::killAll() {
    client->actor->killAll();
}

rsg::Actor* rsg::Actor::forPid(int pid) {
    int64_t addr = client->actor->forPid(pid);
    if(addr == -1)
        return 0;
    else
        return new Actor(addr);
}

void actorRunner(std::function<int(void *)> code, std::string networkName, void *data ) {
    //as client is thread_local, it is a fresh client here. So, we init it:
    client = new RsgClient(networkName);
    
    int pid = rsg::this_actor::getPid();

    debug_spawn_client("Actor [%d] running", pid);
    try {
        code(data);
    } catch(apache::thrift::TApplicationException &ex) {
        std::cerr<< "apache::thrift::TApplicationException in thread : " << ex.what() << std::endl;
    } catch(apache::thrift::transport::TTransportException &ex) {
        //std::cerr<< "apache::thrift::transport::TTransportException in thread : " << ex.what() << std::endl;
        // this exeption occure when the process have been killed on the server by another process.
    }
    //if the program forget to quit at the end of the thread, we do it for him.
    if( client != 0)
        rsg::this_actor::quit();
    debug_spawn_client("Actor [%d] quit", pid);
}

rsg::Actor *rsg::Actor::createActor(std::string name, rsg::HostPtr host, std::function<int(void *)> code, void *data) {
    std::string networkName;
    client->actor->createActorPrepare(networkName);
    
    std::thread *nActor = new std::thread(actorRunner, code, networkName, data);
    child_threads.insert(nActor);
    unsigned long int addr = client->actor->createActor(name, host->p_remoteAddr, 10);//TODO:David: why 10?
    rsg::Actor *act = new Actor(addr);
    
    rsg::Actor *self = rsg::Actor::self();
    char *cstr_name = self->getName();
    delete self;
    debug_spawn_client("Actor [%d]%s creates thread [%d]", this_actor::getPid(), cstr_name, act->getPid());
    free(cstr_name);
    
    return act;
}

rsg::Actor *rsg::Actor::self() {
    unsigned long int addr = client->actor->selfAddr();
    rsg::Actor *act = new Actor(addr);
    return act;
}

int rsg::this_actor::getPid() {
    return client->actor->this_actorGetPid();
}

int rsg::this_actor::getPPid() {
    return client->actor->this_actorGetPPid();
}


int rsg::this_actor::fork(std::string childName) {
    long int hostAddr = Host::current()->p_remoteAddr;

    std::string networkName;
    client->actor->createActorPrepare(networkName);
    
    debug_spawn_client("FORK prepared: %s", networkName.c_str());
    
    debug_spawn_client("FORK go");
    pid_t realSystemPid = ::fork();
    if(0 == realSystemPid) { // Child
        //when a fork happens, all other threads do not exist
        //but local data is still here
        debug_spawn_client("[child] FORK go");
        
        client = new RsgClient(networkName);
        
        debug_spawn_client("[child] FORK go");
        deconnected_threads.clear();
        debug_spawn_client("[child] FORK go");
        child_threads.clear();
        debug_spawn_client("[child] FORK go");
        
        return 0;
    }
    debug_spawn_client("[PARENT] FORK AFTER %d", realSystemPid);
    
    unsigned long long newActorAddr = client->actor->createActor(childName, hostAddr, 10);//TODO:David: why 10?
   
    debug_spawn_client("[PARENT] FORK AFTER create actor");
    
    int newPid = client->actor->getPid(newActorAddr);
   
    debug_spawn_client("[PARENT] FORK AFTER child pid: %d", newPid);
    
    rsg::Actor *self = rsg::Actor::self();
    char *c_name = self->getName();
    std::string name(c_name);
    free(c_name);
    delete self;
    debug_spawn_client(" Actor [%d]%s  created fork [%d]%s", this_actor::getPid(), name.c_str(), newPid, childName.c_str());
    return newPid;
}

bool rsg::Actor::valid() const { 
    return client->actor->isValideActor(this->p_remoteAddr);
}

rsg::Actor::~Actor() {
    client->actor->deleteActor(this->p_remoteAddr);
}
