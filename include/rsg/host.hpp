/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_HOST_HPP_
#define SRC_RSG_HOST_HPP_

#include <boost/unordered_map.hpp>
#include <xbt/string.hpp>

#include "rsg/RsgServiceImpl.h"


namespace simgrid {
namespace rsg {

  class Engine;

  class Host {
  	friend Engine;
  private:
  	Host(const char *name, unsigned long int remoteAddr);
    Host(const std::string name, unsigned long int remoteAddr);
  public:
    simgrid::xbt::string const& name() const { return name_; }
    double speed();
    ~Host();

  public:
    static Host& by_name(std::string name);
    static Host& current();
  protected :
    static void shutdown(); /* clean all globals */

  private:
    simgrid::xbt::string name_;
    unsigned long int p_remoteAddr = 0;

  private :
    static void initNetworkService();

    static Host *pSelf;
    static boost::unordered_map<std::string, Host *> *hosts;
    static boost::shared_ptr<RsgHostClient> pHostService;

  };

} // namespace simgrid::rsg
} // namespace simgrid

#endif /* SRC_RSG_HOST_HPP_ */
