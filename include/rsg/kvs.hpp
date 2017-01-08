/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_KVS_HPP_
#define SRC_RSG_KVS_HPP_

#include <string>

namespace simgrid {
  namespace rsg {
    namespace kvs {

      void get(std::string& _return, const std::string& key);
      void remove(const std::string& key);
      void replace(const std::string& key, const std::string& data);
      void insert(const std::string& key, const std::string& data);

    } //namespace kvs
  } //namespace rsg
} // namespace simgrid

#endif /* SRC_RSG_KVS_HPP_ */
