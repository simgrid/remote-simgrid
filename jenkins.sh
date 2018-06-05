#!/usr/bin/env sh

# Bash sanity
set -eux

basedir=$(realpath .)
builddir=$(realpath ./build)

# Build
rm -rf ${builddir}
mkdir ${builddir}
cd ${builddir}
cmake .. \
    -DSimGrid_PATH="${SIMGRID_PATH:=/builds/simgrid_install}"
make -j $(nproc)

# Test
ctest -T test \
      --output-on-failure --no-compress-output \
      -E 'turnon_turnoff|actor_kill_pid' || true

# Retrieve result
if [ -f ${builddir}/Testing/TAG ] ; then
    resultdir=$(head -n 1 ${builddir}/Testing/TAG)
    resultdir=$(realpath ${builddir}/Testing/${resultdir})

    xsltproc -o ${basedir}/CTestResults.xml \
             ${basedir}/tools/cmake/test/ctest2junit.xsl \
             ${resultdir}/Test.xml
fi
