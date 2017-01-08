#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <boost/scoped_ptr.hpp>
#include <unordered_map>
#include <zmq.hpp>

#include "TZmqServer.hpp"

using boost::shared_ptr;
using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TProtocol;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;

namespace apache { namespace thrift { namespace server {

//  Receive 0MQ string from socket and convert into string
static std::string
s_recv (zmq::socket_t & socket) {

    zmq::message_t message;
    socket.recv(&message);

    return std::string(static_cast<char*>(message.data()), message.size());
}
//  Sends string as 0MQ string, as multipart non-terminal
static bool
s_sendmore (zmq::socket_t & socket, const std::string & string) {

    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

    bool rc = socket.send (message, ZMQ_SNDMORE);
    return (rc);
}
//  Convert string to 0MQ string and send to socket
static bool
s_send (zmq::socket_t & socket, const std::string & string) {

    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

    bool rc = socket.send (message);
    return (rc);
}



void *TZmqServer::router_thread(void *arg)
{
    (void)arg;
    zmq::socket_t frontend(TZmqServer::getContext(), ZMQ_ROUTER);
    frontend.setsockopt(ZMQ_ROUTER_MANDATORY, 1);
    frontend.bind("ipc:///tmp/rsg.frontend.ipc");

    zmq::socket_t controller(TZmqServer::getContext(), ZMQ_REP);
    controller.bind("inproc://router.inproc");

    std::unordered_map<std::string, zmq::socket_t*> worker_queue;

    std::vector<zmq::pollitem_t> sockets;
    sockets.push_back({ (void*)frontend, 0, ZMQ_POLLIN, 0 });
    sockets.push_back({ (void*)controller, 0, ZMQ_POLLIN, 0 });
    
    bool exit_now = false;
    bool ret;
    
    while (!exit_now) {
        //check coherence
        assert(sockets.size() == worker_queue.size()+2);
        
        debug_server_stream<<"[ROUTER] NUM CLIENT "<< sockets.size()<<debug_server_stream_end;
        
        zmq::poll(sockets.data(), sockets.size(), -1);

        //frontend
        if (sockets[0].revents & ZMQ_POLLIN) {
            //  Now get next client request, route to corresponding worker
            //  Client request is [address][empty][request]
            std::string client_addr = s_recv(frontend);
            {
                zmq::message_t message;
                ret = frontend.recv(&message);assert(ret==true);
                assert(message.size() == 0);
            }
            zmq::message_t request;
            ret = frontend.recv(&request);assert(ret==true);

            if( worker_queue.count(client_addr) == 0 ) {
                std::string addr = std::string("inproc://backend.")+client_addr+".inproc";
                debug_server_stream<<"[ROUTER] worker create ++++++BIND+++++ @"<<addr<<debug_server_stream_end;
                zmq::socket_t* backend = new zmq::socket_t(TZmqServer::getContext(), ZMQ_PAIR);
                
                backend->bind(addr);
                worker_queue[client_addr] = backend;
                
                sockets.push_back({ (void*)*backend, 0, ZMQ_POLLIN, 0 });
            }
            
            debug_server_stream<<"[ROUTER] sending to: backend~"<<client_addr<<debug_server_stream_end;
            //NOTE for developpers: if the next call is blocking it means that a client (TZmqClient) tries to connect while there is no server (TZmqServer) ready to accept it.
            //Before creating a client (ie. an Actor) always create first the server.
            ret = worker_queue[client_addr]->send(request);assert(ret==true);
            debug_server_stream<<"[ROUTER] sending to: backend~"<<client_addr<<" DONE"<<debug_server_stream_end;
        }
        
        //  Handle worker activity on backends
        for(unsigned i=2; i-2 < worker_queue.size(); i++) {
            if (sockets[i].revents & ZMQ_POLLIN) {
                debug_server_stream<<"[ROUTER] back act "<<i<<debug_server_stream_end;
                zmq::socket_t* backend = (zmq::socket_t*)&(sockets[i].socket);
                
                //get client name
                char url[1024];
                size_t size = sizeof(url);
                backend->getsockopt(ZMQ_LAST_ENDPOINT, &url, &size );
                url[size - sizeof(".inproc")] = '\0';
                char* client_addr = url+sizeof("inproc://backend.")-1;
                
                debug_server_stream<<"[ROUTER] recv from backend~"<<client_addr<<debug_server_stream_end;
                zmq::message_t reply;
                ret = backend->recv(&reply);assert(ret==true);
                
                debug_server_stream<<"[ROUTER] forward to front~"<<client_addr<<debug_server_stream_end;
                
                if(reply.size()==0)
                    continue;
                
                ret = s_sendmore(frontend, client_addr);assert(ret==true);
                ret = s_sendmore(frontend, "");assert(ret==true);
                ret = frontend.send(reply);assert(ret==true);
            }
        }
        
        //controller
        // it is tested at the end as it may modify the sockets structure
        if (sockets[1].revents & ZMQ_POLLIN) {
            std::string request = s_recv(controller);
            if(request.substr(0, 6) == "CLOSE:") {
                std::string toclose = "inproc://backend."+request.substr(6)+".inproc";
                //find the socket to close
                for(unsigned i=sockets.size()-1; i >= 2 ; i--) {
                    zmq::socket_t* backend = (zmq::socket_t*)&(sockets[i].socket);
                    char url[1024];
                    size_t size = sizeof(url);
                    backend->getsockopt(ZMQ_LAST_ENDPOINT, &url, &size);
                    if(toclose == std::string(url)) {
                        sockets.erase(sockets.begin()+i);
                        delete worker_queue[request.substr(6)];
                        worker_queue.erase(request.substr(6));
                        if(sockets.size() == 2)//no more backends
                            exit_now = true;
                        break;
                    }
                }
                s_send(controller, "DONE");
            } else {
                throw std::runtime_error("[ROUTER] ERROR: unrecognized command");
            }
        }
    }

    debug_server_stream<<"[ROUTER] BYE BYE"<<debug_server_stream_end;
    return 0;
}

//We add an integer on every response.
// This integer is used to send asynchonous signals (like kill).
// You can add more signals, don't forget to respect the standard UNIX signals.
class TSignalServerProtocol : public TProtocolDecorator {
    public:
        TSignalServerProtocol(shared_ptr<TProtocol> _protocol)
            : TProtocolDecorator(_protocol) {}

        virtual ~TSignalServerProtocol() {}

        uint32_t writeMessageBegin_virt(const std::string& _name,
                                                      const TMessageType _type,
                                                      const int32_t _seqid) {
            if (_type == T_REPLY) {
                writeI32_virt(0);
                return TProtocolDecorator::writeMessageBegin_virt(_name,
                                                      _type,
                                                      _seqid);
            } else {
                return TProtocolDecorator::writeMessageBegin_virt(_name, _type, _seqid);
            }
        }
};


bool TZmqServer::serveOne(int recv_flags) {
    debug_server_stream << "[TZmqServer " << name_ << "] recving..." << debug_server_stream_end;
    zmq::message_t msg;
    bool received = sock_.recv(&msg, recv_flags);
    debug_server_stream << "[TZmqServer " << name_ << "] RECVD! " << msg.size() << debug_server_stream_end;
    if (!received) {
        return false;
    }
    shared_ptr<TMemoryBuffer> inputTransport(new TMemoryBuffer((uint8_t*)msg.data(), msg.size()));
    shared_ptr<TMemoryBuffer> outputTransport(new TMemoryBuffer());
    shared_ptr<TProtocol> inputProtocol(
        inputProtocolFactory_->getProtocol(inputTransport));
    shared_ptr<TProtocol> outputProtocol(
        outputProtocolFactory_->getProtocol(outputTransport));
    shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer);

    shared_ptr<TSignalServerProtocol> outout = shared_ptr<TSignalServerProtocol>(new TSignalServerProtocol(outputProtocol));
    processor_->process(inputProtocol, outout, NULL);

    debug_server_stream << "[TZmqServer " << name_ << "] processed, now sending back! " << debug_server_stream_end;
    uint8_t* buf;
    uint32_t size;
    outputTransport->getBuffer(&buf, &size);
    msg.rebuild(size);
    std::memcpy(msg.data(), buf, size);
        debug_server_stream << "[TZmqServer " << name_ << "] sending: " << size << debug_server_stream_end;
    bool ret = sock_.send(msg);assert(ret==true);

  return true;
}



TZmqServer::~TZmqServer() {
    //tell the controller to free resources related to the connection
    debug_server_stream <<"EXITING TZmqServer"<<name_ << *server_exit_ <<debug_server_stream_end;

    //if the process has been killed
    // then send a message back
    if( *server_exit_ == 0 ) {
         debug_server_stream <<"EXITING TZmqServer "<<name_ << ". A kill is emitted."<< *server_exit_ <<debug_server_stream_end;
        zmq::message_t msg(4);

        int32_t net = (int32_t)TNetworkBigEndian::toWire32((int32_t)SIGKILL );
        std::memcpy(msg.data(), &net, 4);

        bool ret = sock_.send(msg);assert(ret==true);
    }

    zmq::socket_t controller(TZmqServer::getContext(), ZMQ_REQ);
    controller.connect("inproc://router.inproc");

    std::string sreq = "CLOSE:"+name_;
    zmq::message_t message(sreq.size());
    memcpy (message.data(), sreq.data(), sreq.size());
    bool ret = controller.send (message);assert(ret==true);

    zmq::message_t repl;
    ret = controller.recv(&repl);assert(ret==true);
    //     debug_server_stream <<"REPONSE DONE =="<< std::string(static_cast<char*>(repl.data()), repl.size())<<debug_server_stream_end;
    debug_server_stream <<"EXITING TZmqServer DONE "<<name_ <<debug_server_stream_end;
}









}}} // apache::thrift::server
