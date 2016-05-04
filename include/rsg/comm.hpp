/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_COMM_HPP_
#define SRC_RSG_COMM_HPP_

#include <boost/unordered_map.hpp>
#include <xbt/string.hpp>

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
    static Comm &send_init(Actor *sender, Mailbox &dest);
    static Comm &send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, int simulatedByteAmount);
    static Comm &recv_init(Actor *receiver, Mailbox &from);
    static rsg::Comm &recv_async(rsg::Actor *receiver, rsg::Mailbox &from, void **data);
    static rsg::Comm &send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size,int simulatedByteAmount);

    void start();
    void wait();
    void wait(double timeout);

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
    static void initNetworkService();
    static boost::shared_ptr<RsgCommClient> pCommService;
  };

} // namespace simgrid::rsg
} // namespace simgrid

#endif /* SRC_RSG_HOST_HPP_ */
