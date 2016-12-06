/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_SERVER_TZMQSERVER_H_
#define _THRIFT_SERVER_TZMQSERVER_H_ 1

#include <zmq.hpp>
#include <thrift/server/TServer.h>
#include <iostream>
#include <mutex>
#include <simgrid/s4u.hpp>
#include "../common.hpp"

namespace apache { namespace thrift { namespace server {

class TZmqServer : public TServer {
 public:
  
/*
Network architecture:
- A client connect to the frontend server (ipc:///tmp/rsg.frontend.ipc)
- The client give a unique name to his session using an identity
- The frontend server receive data on the ROUTER socket
- it forwards the data to a backend on a inproc socket named after the name of the session (inproc://backend.NAME.inproc)
- all sockets use the REP/REQ model (clients wait for a reponse of the server before replying)

TODO: add a second frontend for tcp connections.
*/
  static void* router_thread(void*arg);
  
  static zmq::context_t& getContext() {
    static zmq::context_t instance;
    return instance;
  }

    static void get_new_endpoint(std::string& new_name) {
        static std::mutex next_name_id_mutex;
        static unsigned next_name_id = 0;
        
        next_name_id_mutex.lock();
        unsigned id = next_name_id++;
        next_name_id_mutex.unlock();
        new_name = std::string("Proc")+std::to_string(id);
    }

  
  TZmqServer(
      boost::shared_ptr<TProcessor> processor, const std::string name, bool* server_exit)
    :  TServer(processor)
    , processor_(processor)
    , zmq_type_(ZMQ_PAIR)
    , sock_(TZmqServer::getContext(), zmq_type_)
    , server_exit_(server_exit)
    , name_(name)
  {
      debug_server_stream<<"[TZmqServer "<<name_<<"] ++++++CONNECT+++++ @"<<std::string("inproc://backend.")+name+".inproc"<<debug_server_stream_end;
      
      sock_.connect(std::string("inproc://backend.")+name+".inproc");
  }
  ~TZmqServer();

  bool serveOne(int recv_flags = 0);
  void serve() {
    bool ret = true;
    while(!(*server_exit_) && ret) {
        ret = serveOne();
        assert(ret==true);
    }
  }

  zmq::socket_t& getSocket() {
    return sock_;
  }

 private:
  boost::shared_ptr<TProcessor> processor_;
  int zmq_type_;
  zmq::socket_t sock_;
  bool* server_exit_;
  std::string name_;
};

}}} // apache::thrift::server

#endif // #ifndef _THRIFT_SERVER_TZMQSERVER_H_
