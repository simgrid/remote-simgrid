#include <iostream>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <zmq.h>

int retrieve_status(const std::string & hostname, uint16_t port,
                    const std::string & request_str, int timeout) {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(int));

    std::string connect_str = "tcp://" + hostname + ":" + std::to_string(port);
    zmq_connect(socket, connect_str.c_str());
    zmq_send(socket, request_str.c_str(), request_str.size(), 0);

    zmq_pollitem_t items[] = { {socket, 0, ZMQ_POLLIN, 0} };
    if (zmq_poll(items, 1, timeout) > 0) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        if (zmq_msg_recv(&msg, socket, 0) != -1) {
            std::string reply_str((char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
            printf("%s\n", reply_str.c_str());
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

int main(int argc, char ** argv) {
    std::string hostname = "localhost";
    uint16_t port = 4242;
    std::string request = "status all";
    int timeout = 500;

    namespace po = boost::program_options;
    po::options_description desc("Options description");
    desc.add_options()
            ("help", "print usage message")
            ("waiting-init",
             "only retrieve servers waiting for initial connection")
            ("hostname,h", po::value(&hostname),
             "rsg_server's expected hostname")
            ("port,p", po::value(&port),
             "rsg_server's expected status port")
            ("timeout", po::value(&timeout),
             "request timeout in milliseconds")
            ;

    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(),
                  vm); // throws on error

        if (vm.count("help") > 0) {
            printf("Usage : %s [options]\n", argv[0]);
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("waiting-init")) {
            request = "status waiting_init";
        }

        po::notify(vm);
    }
    catch(boost::program_options::required_option& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    catch(boost::program_options::error& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    int res = retrieve_status(hostname, port, request, timeout);
    return res;
}
