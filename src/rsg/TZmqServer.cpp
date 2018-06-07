#include <unordered_map>

#include <boost/algorithm/string/join.hpp>
#include <boost/scoped_ptr.hpp>

#include <zmq.hpp>

#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>

#include "TZmqServer.hpp"

using boost::shared_ptr;
using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TProtocol;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;

namespace apache { namespace thrift { namespace server {

// initialize static attributes
TZmqServer::SharedConnectionsStatus TZmqServer::s_connection_status;

std::string TZmqServer::ConnectionStatus::as_json() const
{
    std::string waiting_init = std::string(R"("waiting_init":)") +
            (waiting_for_initial_connection? "true" : "false");

    std::vector<std::string> env_vector;
    for (auto it : expected_env) {
        env_vector.push_back("\"" + it.first + R"(":")" + it.second + "\"");
    }
    std::string env_as_string = boost::join(env_vector, ",");
    std::string expected_env = R"("expected_env":{)" + env_as_string + "}";

    std::vector<std::string> main_vector = {waiting_init, expected_env};

    return "{" + boost::join(main_vector, ",") + '}';
}

TZmqServer::SharedConnectionsStatus::SharedConnectionsStatus() {}
TZmqServer::SharedConnectionsStatus::~SharedConnectionsStatus() {}

void TZmqServer::SharedConnectionsStatus::insert_new_connection(const std::string & name)
{
    mutex.lock();
    ConnectionStatus status;
    status.waiting_for_initial_connection = true;
    status.expected_env = {{"RsgRpcNetworkName", name}};
    statuses[name] = status;
    mutex.unlock();
}

void TZmqServer::SharedConnectionsStatus::mark_connection_as_initialized(const std::string &name)
{
    mutex.lock();
    auto it = statuses.find(name);
    xbt_assert(it != statuses.end(), "Internal inconsistency");

    ConnectionStatus & status = it->second;
    status.waiting_for_initial_connection = false;
    mutex.unlock();
}

std::map<std::string, TZmqServer::ConnectionStatus> TZmqServer::SharedConnectionsStatus::all()
{
    mutex.lock();
    auto copy = statuses;
    mutex.unlock();

    return copy;
}

std::map<std::string, TZmqServer::ConnectionStatus> TZmqServer::SharedConnectionsStatus::waiting_init()
{
    auto connections = all();
    std::map<std::string, TZmqServer::ConnectionStatus> res;

    for (auto it : connections) {
        std::string key = it.first;
        ConnectionStatus value = it.second;

        if (value.waiting_for_initial_connection) {
            res[key] = value;
        }
    }
    return res;
}

std::string TZmqServer::SharedConnectionsStatus::as_json(const std::map<std::string, TZmqServer::ConnectionStatus> &statuses)
{
    std::vector<std::string> v;
    for (auto it : statuses) {
        v.push_back(it.second.as_json());
    }
    return "[" + boost::join(v, ",") + "]";
}


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
        debug_server_print("[ROUTER] NUM CLIENT %zu", sockets.size());
        
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
                debug_server_print("[ROUTER] worker create ++++++BIND+++++ @%s", addr.c_str());
                zmq::socket_t* backend = new zmq::socket_t(TZmqServer::getContext(), ZMQ_PAIR);
                
                backend->bind(addr);
                worker_queue[client_addr] = backend;
                
                sockets.push_back({ (void*)*backend, 0, ZMQ_POLLIN, 0 });

                s_connection_status.mark_connection_as_initialized(client_addr);

                debug_server_print("Connections waiting for initialization: %s",
                    SharedConnectionsStatus::as_json(s_connection_status.waiting_init()).c_str());;
            }
            
            debug_server_print("[ROUTER] sending to: backend~%s", client_addr.c_str());
            //NOTE for developpers: if the next call is blocking it means that a client (TZmqClient) tries to connect while there is no server (TZmqServer) ready to accept it.
            //Before creating a client (ie. an Actor) always create first the server.
            ret = worker_queue[client_addr]->send(request);assert(ret==true);
            debug_server_print("[ROUTER] sending to: backend~%s", client_addr.c_str());
        }
        
        //  Handle worker activity on backends
        for(unsigned i=2; i-2 < worker_queue.size(); i++) {
            if (sockets[i].revents & ZMQ_POLLIN) {
                debug_server_print("[ROUTER] back act %d", i);
                zmq::socket_t* backend = (zmq::socket_t*)&(sockets[i].socket);
                
                //get client name
                char url[1024];
                size_t size = sizeof(url);
                backend->getsockopt(ZMQ_LAST_ENDPOINT, &url, &size );
                url[size - sizeof(".inproc")] = '\0';
                char* client_addr = url+sizeof("inproc://backend.")-1;
                
                debug_server_print("[ROUTER] recv from backend~%s", client_addr);
                zmq::message_t reply;
                ret = backend->recv(&reply);assert(ret==true);
                
                debug_server_print("[ROUTER] forward to front~%s", client_addr);
                
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

    debug_server_print("[ROUTER] BYE BYE");
    return 0;
}

zmq::context_t &TZmqServer::getContext() {
    static zmq::context_t instance;
    return instance;
}

void TZmqServer::get_new_endpoint(std::string &new_name) {
    static std::mutex next_name_id_mutex;
    static unsigned next_name_id = 0;

    next_name_id_mutex.lock();
    unsigned id = next_name_id++;
    next_name_id_mutex.unlock();
    new_name = std::string("Proc")+std::to_string(id);
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
    debug_server_print("[TZmqServer %s] recving...", name_.c_str());
    zmq::message_t msg;
    bool received = sock_.recv(&msg, recv_flags);
    debug_server_print("[TZmqServer %s] recv %zu bytes", name_.c_str(), msg.size());

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

    debug_server_print("[TZmqServer %s] processed, now sending back!", name_.c_str());
    uint8_t* buf;
    uint32_t size;
    outputTransport->getBuffer(&buf, &size);
    msg.rebuild(size);
    std::memcpy(msg.data(), buf, size);

    debug_server_print("[TZmqServer %s] sending %d", name_.c_str(), size);
    bool ret = sock_.send(msg);
    assert(ret==true);

    return true;
}

void TZmqServer::serve() {
    bool ret = true;

    if (!(*server_exit_)) {
        ret = serveOne();
        if (ret) {

        }
    }

    while(!(*server_exit_) && ret) {
        ret = serveOne();
        assert(ret==true);
    }
}

zmq::socket_t &TZmqServer::getSocket() {
    return sock_;
}

TZmqServer::TZmqServer(boost::shared_ptr<TProcessor> processor, const std::string name, bool *server_exit)
    :  TServer(processor)
    , processor_(processor)
    , zmq_type_(ZMQ_PAIR)
    , sock_(TZmqServer::getContext(), zmq_type_)
    , server_exit_(server_exit)
    , name_(name)
{
    // Store that the connection is not done yet.
    s_connection_status.insert_new_connection(name);

    std::string endpoint = "inproc://backend." + name + ".inproc";
    debug_server_print("[TZmqServer %s] ++++++CONNECT+++++ @%s",
                       name.c_str(), endpoint.c_str());
    sock_.connect(endpoint);
}

TZmqServer::~TZmqServer() {
    //tell the controller to free resources related to the connection
    debug_server_print("[TZmqServer %s] exiting %d", name_.c_str(), *server_exit_);

    //if the process has been killed
    // then send a message back
    if( *server_exit_ == 0 ) {
        debug_server_print("[TZmqServer %s] exiting. A kill is emitted. %d", name_.c_str(), *server_exit_);
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
    debug_server_print("[TZmqServer %s] exiting done", name_.c_str());
}

}}} // apache::thrift::server
