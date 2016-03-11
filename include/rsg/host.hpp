/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_HOST_HPP_
#define SRC_RSG_HOST_HPP_

#include <boost/unordered_map.hpp>
#include "rsg/engine.hpp"

namespace simgrid {
  namespace rsg {

    class Host {

    private:
    	Host(const char *name);
      Host(const std::string name);

    public:
      static Host *current();
      const char * name() { return p_name.c_str(); }
      ~Host();
    private:
      std::string p_name;
      static Host *p_self;
    };

  } // namespace simgrid::rsg
} // namespace simgrid

#endif /* SRC_RSG_HOST_HPP_ */
