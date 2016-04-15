#!/usr/local/bin/thrift --gen cpp

namespace cpp RsgService

service RsgService {
  void sleep(1:double duration)
  void execute(1:double flops)
  void close()
}

service RsgMailBox {

}
