#!/usr/local/bin/thrift --gen cpp

namespace cpp RsgService

service RsgService {
  void sleep(1:double duration)
  void execute(1:double flops)
  void send(1:i64 mbAddr,2:string content, 3:i64 simulatedSize)
  string recv(1:i64 mbAddr)
  void close()
}

service RsgMailbox {
  i64 mb_create(1:string name)
}
