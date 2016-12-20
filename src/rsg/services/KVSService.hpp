#include "../services.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"
#include <iostream>
#include <string>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

#include "RsgService_types.h"
#include "RsgKVS.h"

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

using boost::shared_ptr;

using namespace  ::RsgService;

namespace simgrid  {
  namespace rsg {
    /**
     * This class is a singleton, so It is easier to load data from a file once without haveing dynamics checks.
     * NOTE: I am willing to discuss to find a better solution than a singleton.
     */
    
    /**
     *RsgKVSHandler this service allows the user to store custom data unrelated to simgrid work.
     *It is a simple KVS store accessible by each actor.
     *When multithreading is activated in simgrid any actor can acquire a lock on the kvs. To keep the simulation consistent,
     *when a process have a lock on the mutex it must not makes any call to any rsg function.
     *Otherwise there is a strong probability of triggering a deadlock among simgrid processes.
     */
    class RsgKVSHandler : virtual public RsgKVSIf {
    public:
      static shared_ptr<RsgKVSHandler> getInstance();

      void get(std::string& _return, const std::string& key);
      void remove(const std::string& key);
      void replace(const std::string& key, const std::string& data);
      void insert(const std::string& key, const std::string& data);

      void take_lock();
      void release_lock();

      ~RsgKVSHandler();

    private:
      static shared_ptr<RsgKVSHandler> instance;
      std::mutex sync;

      std::unordered_map<std::string, std::string> store;
      RsgKVSHandler();
  };


  } /* namespace rsg */ 
} /* namespace simgrid */ 