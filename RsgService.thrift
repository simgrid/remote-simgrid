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
