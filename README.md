# Remote SimGrid

RSG is (will be) a simple solution to execute your distributed
application on top of (SimGrid)[http://simgrid.org/]. You just have to
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
algorithms or applications. Remote SimGrid is currently under heavy
development. It is expected to be usable in production by the end of
the year (2015).

## Contact and Licence 

If you are interested, please drop us a line on the
simgrid-user@lists.gforge.inria.fr mailing list.

SimGrid is released under LGPL while RemoteSimGrid is released under
the GNU Affero Licence. Please see the source packages for more
information.

## Build dependencies

- SimGrid       -- http://simgrid.org/
