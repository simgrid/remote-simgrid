/* Copyright (c) 2006-2016. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#pragma once

#include <xbt.h>

#include <condition_variable>
#include <rsg/mutex.hpp>

namespace simgrid {
    namespace rsg {
        
        class Mutex;
        
        class XBT_PUBLIC ConditionVariable {
            
        public:
            ConditionVariable();
            ~ConditionVariable();
            
            /**
            * Wait functions
            */
            void wait(Mutex *mutex);
            std::cv_status wait_for(Mutex *mutex, double time);
            
            /**
            * Notify functions
            */
            void notify_one();
            void notify_all();
            
            void destroy();
        private:
            unsigned long int p_remoteAddr = 0;
            
        };
    }} // namespace simgrid::s4u
