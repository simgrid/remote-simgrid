/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_COMM_HPP_
#define SRC_RSG_COMM_HPP_

#include "client/RsgClient.hpp"
#include "client/multiThreadedSingletonFactory.hpp"

#include <vector>
#include <boost/unordered_map.hpp>
#include <xbt/string.hpp>
#include <iostream>
#include "rsg/services.hpp"
#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"

namespace simgrid {
    namespace rsg {
        
        class Actor;
        class Mailbox;
        class Comm {
            
        private:
            Comm(unsigned long int remoteAddr);
            
        public:
            ~Comm();
            static Comm &send_init(Mailbox &dest);
            
            /* Unsupported */
            static Comm &send_async(rsg::Mailbox &dest, void *data, int simulatedByteAmount);
            
            /* Unsupported */
            static rsg::Comm &send_async(rsg::Mailbox &dest, void *data);
            
            static rsg::Comm &send_async(rsg::Mailbox &dest, void *data, size_t size, int simulatedByteAmount);
            static rsg::Comm &send_async(rsg::Mailbox &dest, void *data, size_t size);
            
            static Comm &recv_init(Mailbox &from);
            static rsg::Comm &recv_async(rsg::Mailbox &from, void **data);
            
            void start();
            void wait();
            void wait(double timeout);
            bool test();
            
            /*! tanke a range of s4u::Comm* (last excluded) and return when one of them is finished. The return value is an iterator on the finished Comms. */
            template<class I> static
            I wait_any(I first, I last)
            {
                rsgCommIndexAndData _return;
                Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
                RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
                std::vector<int64_t> comms;
                for(auto it = first; it != last; it++) {
                    comms.push_back((*it)->p_remoteAddr);
                }
                
                commService.wait_any(_return, comms);
                auto terminatedComm = std::next(first, _return.index);
                if ((*terminatedComm)->dstBuff_ != NULL) {
                    char * chars = (char*) malloc(_return.data.size());
                    memcpy(chars, _return.data.c_str(), _return.data.size());
                    *(void**) (*terminatedComm)->dstBuff_ = (char *) chars;
                } else {
                    std::cerr << "cannot wait any with an empty buffer " << std::endl;
                }
                
                return terminatedComm;
            }
            
            /*! tanke a range of s4u::Comm* (last excluded) and return when one of them is finished. The return value is an iterator on the finished Comms. */
            template<class I> static
            I wait_any_for(I first, I last, double timeout)
            {
                rsgCommIndexAndData _return;
                Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
                RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
                std::vector<int64_t> comms;
                for(auto it = first; it != last; it++) {
                    comms.push_back((*it)->p_remoteAddr);
                }
                commService.wait_any_for(_return, comms, timeout);
                if(_return.index == -1)  {
                    return last;
                }
                
                auto terminatedComm = std::next(first, _return.index);
                if ((*terminatedComm)->dstBuff_ != NULL) {
                    char * chars = (char*) malloc(_return.data.size());
                    memcpy(chars, _return.data.c_str(), _return.data.size());
                    *(void**) (*terminatedComm)->dstBuff_ = (char *) chars;
                } else {
                    std::cerr << "cannot wait any with an empty buffer " << std::endl;
                }
                
                return terminatedComm;
            }
            
            public :
            void setRate(double rate);
            
            public :
            void setSrcData(void * buff);
            void setSrcDataSize(size_t size);
            void setSrcData(void * buff, size_t size);
            void setDstData(void ** buff);
            void setDstData(void ** buff, size_t size);
            size_t getDstDataSize();
            
        private:
            unsigned long int p_remoteAddr = 0;
            
            private :
            void *dstBuff_ = NULL;
            size_t dstBuffSize_ = 0;
            void *srcBuff_ = NULL;
            size_t srcBuffSize_ = 0;
        };
        
    } // namespace simgrid::rsg
} // namespace simgrid

#endif /* SRC_RSG_HOST_HPP_ */
