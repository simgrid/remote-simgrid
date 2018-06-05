/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#pragma once

#include <xbt/string.hpp>
#include <memory>

namespace simgrid {
    namespace rsg {
        
        namespace this_actor {
            extern int fork(std::string childName);
        }
        
        class Actor;
        class Host {
            
            friend Actor;    
            friend int simgrid::rsg::this_actor::fork(std::string childName);
        private:
            Host(const char *name, unsigned long int remoteAddr);
            Host(const std::string name, unsigned long int remoteAddr);
            
        public:
            using Ptr = std::shared_ptr<Host>;
            double speed();
            ~Host();
            
            simgrid::xbt::string const& getName() const { return name_; }
            static Ptr by_name(std::string name);
            static Ptr current();
            int coreCount();
            void turnOn();
            void turnOff();
            bool isOn();
            bool isOff() { return !isOn(); }
            double currentPowerPeak();
            double powerPeakAt(int pstate_index);
            int pstatesCount() const;
            void setPstate(int pstate_index);
            int pstate();
            /** Retrieve the property value (or nullptr if not set) */
            char* property(const char*key);
            void setProperty(const char*key, const char *value);
            protected :
            static void shutdown(); /* clean all globals */
            
        private:
            simgrid::xbt::string name_;
        public:
            unsigned long int p_remoteAddr = 0;
            
        };
        
        using HostPtr = Host::Ptr;
    } // namespace simgrid::rsg


} // namespace simgrid
