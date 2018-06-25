# Remote SimGrid

RSG is (will be) a simple solution to execute your distributed
application on top of [SimGrid](http://simgrid.org/). You just have to
reimplement the little class abstracting every network interactions
that you have down below in your code architecture. This is easy with
the RSG client API.

Then, the RSG server can start your (real) application on top of a
(simulated) platform, enabling you to use the full power of SimGrid as
a perfect testbench for your application.

Enjoy reproducibility, clairvoyance and simplicity while testing your
distributed application or infrastructure !

## Current State

SimGrid is a strong and mature framework to study distributed
algorithms or applications. Remote SimGrid is usable but still lacking
some features. The thing is that we are currently rewriting SimGrid
(since 2015) to make the S4U interface happen. Once this is done, we
will come back to the Remote SimGrid project. It may still work for
you in the meanwhile, who knows.

CI status: [![CircleCI](https://circleci.com/gh/simgrid/remote-simgrid/tree/master.svg?style=svg)](https://circleci.com/gh/simgrid/remote-simgrid/tree/master)

## Contact and License

If you are interested, please drop us a line on the
simgrid-user@lists.gforge.inria.fr mailing list.

SimGrid is released under LGPL while Remote SimGrid is released under
the GNU Affero License. Please see the source packages for more
information.

## Installing Remote SimGrid

- Install [Thrift](http://www.apache.org/dyn/closer.cgi?path=/thrift/0.11.0/thrift-0.11.0.tar.gz) (version 0.11.0)

```bash
wget http://apache.lauf-forum.at/thrift/0.11.0/thrift-0.11.0.tar.gz
tar -xvf thrift-0.11.0.tar.gz
cd thrift-0.11.0
./configure
make
[sudo] make install
```

- If you want to run the tests, thrift have to be compiled with extra compilers flags :

  ```bash
wget http://apache.lauf-forum.at/thrift/0.11.0/thrift-0.11.0.tar.gz
tar -xvf thrift-0.11.0.tar.gz
cd thrift-0.11.0
export CXXFLAGS="-DTHRIFT_SQUELCH_CONSOLE_OUTPUT" && export CFLAGS="-DTHRIFT_SQUELCH_CONSOLE_OUTPUT" && ./configure
make
[sudo] make install
```

- Install [zeromq](http://zeromq.org/intro:get-the-software) 
```bash
wget https://github.com/zeromq/libzmq/releases/download/v4.2.0/zeromq-4.2.0.tar.gz
tar xvf zeromq-4.2.0.tar.gz
(cd zeromq-4.2.0.tar && ./configure && make install) #You may need to be logged with root privileges.
# Once zeromq is installed, you need to get the c++ binding.
git clone https://github.com/zeromq/cppzmq
(cd cppzmq && mv cp *.hpp /usr/local/include/)
```


- Install a modern version of [SimGrid](http://simgrid.org/) (preferably the git version).

```bash
# Fetch the S4U branch
git clone https://github.com/simgrid/simgrid.git --depth 1
cd simgrid
```

```bash
# Configure it
cmake -Denable_documentation=OFF -Denable_smpi=off -DCMAKE_INSTALL_PREFIX=/opt/simgrid .

# Build and install it into /opt (change CMAKE_INSTALL_PREFIX to install elsewhere)
make install
```

- And then compile Remote SimGrid:
```bash
cmake -DSimGrid_PATH=/opt/simgrid -DCMAKE_INSTALL_PREFIX=/opt/rsg .
make
```

There is no way to properly install the soft yet :(
