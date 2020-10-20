# Remote SimGrid

RSG is a simple solution to execute your distributed
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
algorithms or applications.
The current (2020-10) RSG implementation aims at robustness,
all supported features should be tested.

[![pipeline status](https://framagit.org/simgrid/remote-simgrid/badges/master/pipeline.svg)](https://framagit.org/simgrid/remote-simgrid/pipelines)
[![coverage report](https://img.shields.io/codecov/c/github/simgrid/remote-simgrid.svg)](https://codecov.io/gh/simgrid/remote-simgrid)
[![sonar maintainability](https://sonarcloud.io/api/project_badges/measure?project=remote-simgrid&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=remote-simgrid)

## Contact and License

If you are interested, please drop us a line on the
simgrid-user@lists.gforge.inria.fr mailing list.

SimGrid is released under LGPL-2.1 and Remote SimGrid is released under LGPL-3.0.

## Installing Remote SimGrid manually

Install dependencies:
- docopt-cpp: `nix-env -f https://github.com/oar-team/kapack/archive/master.tar.gz -iA docopt_cpp`
- Protobuf: `nix-env -i protobuf`
- SimGrid: `nix-env -f https://github.com/oar-team/kapack/archive/master.tar.gz -iA simgrid324`

Make sure all of them are accessible by pkg-config:
`pkg-config --cflags --libs docopt protobuf simgrid` should
output how to link and include all the libraries.
If this is not the case, update your PKG_CONFIG_PATH environment variable.

Then, Remote SimGrid is built with the [Meson](https://mesonbuild.com/) build system:

```
meson build # --prefix=/install/directory
ninja -C build
ninja install -C build
```
