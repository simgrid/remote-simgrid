#!/usr/local/bin/thrift --gen cpp

namespace cpp RsgService
namespace java RsgService

struct rsgHostCurrentResType {
    1:i64 addr,
    2:string name
}

struct rsgServerRemoteAddrAndPort {
    1:i64 addr,
    2:i32 port
}

struct rsgCommBoolAndData {
    1:bool cond,
    2:binary data
}

struct rsgCommIndexAndData {
    1:i64 index,
    2:binary data
}

enum rsgConditionVariableStatus {
    cv_timeout = 1,
    cv_no_timeout = 2
}    

service RsgEngine {
    double getClock()
}

service RsgActor    {
    void sleep(1:double duration)
    void execute(1:double flops)
    void send(1:i64 mbAddr,2:string content, 3:i64 simulatedSize)
    string recv(1:i64 mbAddr)
    void close()
    void kill(1:i64 mbAddr)
    void killPid(1:i32 pid)
    void join(1:i64 addr)
    string getName(1:i64 addr)
    rsgHostCurrentResType getHost(1:i64 addr)
    i32 getPid(1:i64 addr)
    i32 this_actorGetPid()
    void setAutoRestart(1:i64 addr, 2:bool autorestart)
    void setKillTime(1:i64 addr, 2:double time)
    double getKillTime(1:i64 addr)
    void killAll()
    rsgServerRemoteAddrAndPort createActorPrepare()
    i64 createActor(1:i64 remoteServerAddr, 2:i32 port , 3:string name, 4:i64 host, 5:i32 killTime)
    i64 forPid(1:i32 pid)
    void deleteActor(1:i64 addr)
    bool isValideActor(1:i64 remoteAddr)
}

service RsgMailbox   {
    i64 mb_create(1:string name)
    void setReceiver(1:i64 remoteAddrMailbox, 2:i64 remoteAddrActor)
    i64 getReceiver(1:i64 remoteAddrMailbox)
    bool empty(1:i64 remoteAddrMailbox)
}

service RsgMutex   {
    i64 mutexInit()
    void destroy(1:i64 rmtAddr)
    void lock(1:i64 rmtAddr)
    void unlock(1:i64 rmtAddr)
    bool try_lock(1:i64 rmtAddr)
}

service RsgConditionVariable {
    i64  conditionVariableInit()
    void conditionVariableDestroy(1:i64 remoteAddr)
    void wait(1:i64 remoteAddr, 2:i64 mutexAddr)
    rsgConditionVariableStatus wait_for(1:i64 remoteAddr, 2:i64 mutexAddr, 3:double timeout)
    void notify(1:i64 remoteAddr)
    void notify_all(1:i64 remoteAddr)
}


service RsgHost   {
    i64 by_name(1:string name)
    rsgHostCurrentResType current()
    double speed(1:i64 addr)
    void turnOn(1:i64 addr)
    void turnOff(1:i64 addr)
    bool isOn(1:i64 addr)
    double currentPowerPeak(1:i64 addr)
    double powerPeakAt(1:i64 addr, 2:i32 pstate_index)
    i32 pstatesCount(1:i64 addr)
    void setPstate(1:i64 addr, 2:i32 pstate_index)
    i32 pstate(1:i64 addr)
    i32 core_count(1:i64 addr)
    string getProperty(1:i64 remoteAddr, 2:string key)
    void setProperty(1:i64 remoteAddr, 2:string key, 3:binary data)
}

service RsgComm   {
    i64 send_init(1:i64 sender, 2:i64 dest)
    i64 recv_init(1:i64 receiver, 2:i64 from_)
    i64 send_async(1:i64 sender, 2:i64 dest,3:binary data , 4:i64 size, 5:i64 simulatedByteAmount)    
    string waitComm(1:i64 commAddr)
    void start(1:i64 addr)
    void setRate(1:i64 addr, 2:double rate);
    i64 getDstDataSize(1:i64 addr)
    i64 recv_async(1:i64 receiver, 2:i64 from_)
    void setDstData(1:i64 addr)
    void setSrcData(1:i64 addr, 2:binary buff)
    rsgCommIndexAndData wait_any(1:list<i64> comms)
    rsgCommIndexAndData wait_any_for(1:list<i64>  comms, 2:double timeout)
    rsgCommBoolAndData test(1:i64 addr)
}
