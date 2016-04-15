#!/usr/local/bin/thrift --gen cpp

namespace cpp RsgService

service RsgService {
  void sleep(1:i32 duration)
  void close()
}

service RsgMailBox {

}
