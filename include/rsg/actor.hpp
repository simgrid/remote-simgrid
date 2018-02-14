/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <functional>

#include <xbt.h>

#include <rsg/mailbox.hpp>
#include <rsg/host.hpp>
#include <rsg/comm.hpp>


namespace simgrid  {
    namespace rsg {
        
        class Mailbox;
        class Host;
        class Comm;
        
        class Actor {
            friend rsg::Comm;
            friend rsg::Mailbox;
        private: 
            Actor(unsigned long int remoteAddr);
        public:
            static void killAll();
            static Actor *self();
            static Actor *createActor(std::string name, rsg::HostPtr host, std::function<int(void *)> code, void *data);
            static Actor *byPid(int pid);
            static void kill(int pid);
            void kill();
            void join();
            void setAutoRestart(bool autorestart);
            void setKillTime(double time);
            double getKillTime();
            char*getName();
            Host *getHost();
            int getPid();
            int getPPid();
            bool valid() const;
            ~Actor();
            
        private:
            unsigned long int p_remoteAddr = 0;
            rsg::Host *pHost;
        };
        
        namespace this_actor {
            
            // Static methods working on the current actor:
            
            /** Block the actor sleeping for that amount of seconds (may throws hostFailure) */
            XBT_PUBLIC(void) sleep(double duration);
            
            /** Block the actor, computing the given amount of flops */
            XBT_PUBLIC(void) execute(double flop);
            
            /** Block the actor until it gets a message from the given mailbox.
            *
            * See \ref Comm for the full communication API (including non blocking communications).
            */
            XBT_PUBLIC(char*) recv(rsg::Mailbox &chan);
            
            /** Block the actor until it delivers a message of the given simulated size to the given mailbox
            *
            * See \ref Comm for the full communication API (including non blocking communications).
            */	
            XBT_PUBLIC(void) send(rsg::Mailbox &mailbox, const char*content, size_t dataSize);
            
            XBT_PUBLIC(void) send(rsg::Mailbox &mailbox, const char*content, size_t dataSize, size_t simulatedSize);
            
            XBT_PUBLIC(void) quit();
            
            XBT_PUBLIC(int) getPid();

            XBT_PUBLIC(int) getPPid();
            
            XBT_PUBLIC(int) fork(std::string childName);
            
        };
        
    }} // namespace simgrid::rsg
    
    #endif /* SIMGRID_RSG_ACTOR_HPP */
