# Remote SimGrid

RSG is (will be) a simple solution to execute your distributed
application on top of [SimGrid](http://simgrid.org/). You just have to
reimplement the little class abstracting every network interactions
that you have down below in your code architecture. This is easy with
the RSG client API.

Then, the RSG server can start your (real) application on top of a
(simulated) platform, enabling you to use the full power of SimGrid as
a perfect testbench for your application.

Enjoy reproducibility, clairevoyance and simplicity while testing your
distributed application or infrastructure !

## Current State

SimGrid is a strong and mature framework to study distributed
algorithms or applications. Remote SimGrid is usable but still lacking
some features. It is expected to be usable in production by the end of
the year (2015).

## Contact and Licence

If you are interested, please drop us a line on the
simgrid-user@lists.gforge.inria.fr mailing list.

SimGrid is released under LGPL while RemoteSimGrid is released under
the GNU Affero Licence. Please see the source packages for more
information.

## Installing Remote-Simgrid

- Install [Thrift](http://www.apache.org/dyn/closer.cgi?path=/thrift/0.9.3/thrift-0.9.3.tar.gz) (version 0.9.3)

  ```
wget http://apache.lauf-forum.at/thrift/0.9.3/thrift-0.9.3.tar.gz
tar -xvf thrift-0.9.3.tar.gz
cd thrift-0.9.3
./configure
make
[sudo] make install
```

- If you want to run the tests, thrift have to be compiled with extra compilers flags :

  ```bash
  wget http://apache.lauf-forum.at/thrift/0.9.3/thrift-0.9.3.tar.gz
  tar -xvf thrift-0.9.3.tar.gz
  cd thrift-0.9.3
  export CXXFLAGS="-DTHRIFT_SQUELCH_CONSOLE_OUTPUT" && export CFLAGS="-DTHRIFT_SQUELCH_CONSOLE_OUTPUT" && ./configure
  make
  [sudo] make install
  ```


- Install the S4U branch of [SimGrid](http://simgrid.org/) (which in
  turn depends on Boost). S4U is a preview of the future SimGrid v4.

  ```
  # Fetch the S4U branch
  git clone https://github.com/mquinson/simgrid.git --branch S4U --depth 1
  cd simgrid
  ```

  ```
  # Configure it
  cmake -Denable_documentation=OFF -Denable_smpi=off -DCMAKE_INSTALL_PREFIX=/opt/simgrid .

  # Build and install it into /opt (change CMAKE_INSTALL_PREFIX to install elsewhere)
  make install
  ```

- And then compile remote-simgrid:
```
cmake -DSimGrid_PATH=/opt/simgrid -DCMAKE_INSTALL_PREFIX=/opt/rsg .
make
```

There is no way to properly install the soft yet :(
