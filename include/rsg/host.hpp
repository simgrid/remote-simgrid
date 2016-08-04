/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_HOST_HPP_
#define SRC_RSG_HOST_HPP_

#include <boost/unordered_map.hpp>
#include <xbt/string.hpp>

#include "rsg/services.hpp"
#include "rsg/actor.hpp"

namespace simgrid {
    namespace rsg {
        
        namespace this_actor {
            extern int fork();
        }
        
        class Actor;
        class Host {
            
            friend Actor;    
            friend int simgrid::rsg::this_actor::fork();
        private:
            Host(const char *name, unsigned long int remoteAddr);
            Host(const std::string name, unsigned long int remoteAddr);
            
        public:
            double speed();
            ~Host();
            
            simgrid::xbt::string const& name() const { return name_; }
            static Host& by_name(std::string name);
            static Host& current();
            int core_count();
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
            unsigned long int p_remoteAddr = 0;
            
        };
        
    } // namespace simgrid::rsg
} // namespace simgrid

#endif /* SRC_RSG_HOST_HPP_ */
