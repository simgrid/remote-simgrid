INCLUDES=-I/opt/simgrid/include
CPPFLAGS=-g -O0 -fPIC -fno-common $(INCLUDES) -Wall -Wextra -Wunused -Wpointer-arith -Wchar-subscripts -Wcomment  -Wformat -Wwrite-strings -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing -Wno-format-nonliteral -Werror -Wclobbered -Wno-error=clobbered -std=gnu++11 -g3 -fPIC
LDFLAGS=-L/opt/simgrid/lib -lsimgrid -lmicrohttpd

all: simgrid_server simgrid_client
simgrid_client: client/rsg_client.o
	g++ -o $@ $^ $(LDFLAGS)

simgrid_server: server.o
	g++ -o $@ $^ $(LDFLAGS)


client/rsg_client.o: client/rsg_client.cpp
server.o: server.cpp

clean: 
	rm -f server.o simgrid_server