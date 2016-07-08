/* Copyright (c) 2006-2016. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef RSG_COND_VARIABLE_HPP
#define RSG_COND_VARIABLE_HPP

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include "rsg/services.hpp"
#include "rsg/mutex.hpp"

namespace simgrid {
namespace rsg {

class Mutex;

XBT_PUBLIC_CLASS ConditionVariable {
  
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
  void notify();
  void notify_all();
  
  void destroy();
private:
  unsigned long int p_remoteAddr = 0;

};
}} // namespace simgrid::s4u

#endif /* RSG_COND_VARIABLE_HPP */
