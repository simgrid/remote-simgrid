# How to add a new feature

The following steps describes how to add a new feature into remote-simgrid.

## What is a service in remote-simgrid (RSG)

A service is an object which can be called by a client remotely (see rpc server).
In remote simgrid, we use service to access to s4u features such as host or actor etc..

## How to create a new service

The following steps describes how to create new service.

1. First define your service according to the thrift convention into the file `RsgService.thift`.
2. Now you can generate the service skeleton by running cmake command.
3. Thrift create a skeleton class into build/gen-cpp/<>
4. Now create the two files described bellow :
  1. **The header** : `src/rsg/services/<YourServiceName>.hpp`
  1. **The cpp source** : `src/rsg/services/<YourServiceName>.cpp`
5. Once the file a created, you need to add it to the `src/CMakeLists.txt` among the other services. (cmake instruction at line 18).

## Add service to Remote-Simgrid

Once the service is created and the project compile,if you have not implemented the service yet, you may need to modify some functions. Otherwise the project won't compile.

To use the service into the remote process, you need to add the service to the server processor in the file `src/main_server.cpp` into the function `rsg_representative`.
Once the service is added to the server, you will be able to get an instance thanks to the function `template<class ServiceType> ServiceType* Client::serviceClientFactory(std::string name)`.

# Run remote process with gdb or valgring

You can directly run remote process with valgring or gdb. In your deployment file you can define your remote process like :

```xml
<process host="host0" function="client">
   <argument value="valgrind"/>
   <argument value="/home/adfaure/projects/remote-simgrid/build/tests/basic/main_client"/>
</process>
```
