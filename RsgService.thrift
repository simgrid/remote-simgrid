#!/usr/local/bin/thrift --gen cpp

namespace cpp RsgService

struct rsgHostCurrentResType {
  1:i64 addr,
  2:string name
}

service RsgActor {
  void sleep(1:double duration)
  void execute(1:double flops)
  void send(1:i64 mbAddr,2:string content, 3:i64 simulatedSize)
  string recv(1:i64 mbAddr)
  void close()
  string getName(1:i64 addr)
  rsgHostCurrentResType getHost(1:i64 addr)
  i32 getPid(1:i64 addr)
  void setAutoRestart(1:i64 addr, 2:bool autorestart)
  void setKillTime(1:i64 addr, 2:double time)
  double getKillTime(1:i64 addr)
  void killAll()
}

service RsgMailbox {
  i64 mb_create(1:string name)
}

service RsgHost {
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
}

service RsgComm {
  i64 send_init(1:i64 sender, 2:i64 dest)
  i64 recv_init(1:i64 receiver, 2:i64 from_)

  void start(1:i64 addr)
  binary wait(1:i64 addr)
  void setRate(1:i64 addr, 2:double rate);

  i64 getDstDataSize(1:i64 addr)

  i64 recv_async(1:i64 receiver, 2:i64 from_)
  void setDstData(1:i64 addr, 2:i64 size)
  void setSrcData(1:i64 addr, 2:binary buff);


  //void wait_timeout(1:i64 addr, 2:double timeout)
  //i64 send_async(1:i64 sender, 2:i64 dest, 3:i32 simulatedByteAmount)
  //void setSrcDataSize(1:i64 addr, 2:i64 size)
  //void setSrcData(1:i64 addr, 2:binary buff)
  //void setDstData(void ** buff);
}
