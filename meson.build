project('remote-simgrid', 'cpp',
    version: '0.2.0',
    license: 'LGPL-3.0',
    default_options: ['cpp_std=c++11'],
    meson_version: '>=0.37.1'
)

# Dependencies
boost_dep = dependency('boost', required: true)
docopt_dep = dependency('docopt', required: true)
protobuf_dep = dependency('protobuf', required : true)
protoc = find_program('protoc', required : true)
simgrid_dep = dependency('simgrid', required: true)

rsg_deps = [
    boost_dep,
    docopt_dep,
    protobuf_dep,
    simgrid_dep
]

librsg_deps = [
    protobuf_dep,
]

# Generate source files from protobuf schema description.
gen = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
  arguments : [
    '--proto_path=@CURRENT_SOURCE_DIR@',
    '--cpp_out=@BUILD_DIR@',
    '@INPUT@'
  ]
)
protobuf_generated_src = gen.process('rsg.proto')

# Source files
rsg_src = [
    'src/common/assert.hpp',
    'src/common/message.hpp',
    'src/common/network/selector.hpp',
    'src/common/network/selector.cpp',
    'src/common/network/tcp_listener.hpp',
    'src/common/network/tcp_listener.cpp',
    'src/common/network/tcp_socket.hpp',
    'src/common/network/tcp_socket.cpp',
    'src/rsg/main.cpp',
    'src/rsg/serve.cpp',
    'src/rsg/serve.hpp',
    'src/rsg/simulation.cpp',
    'src/rsg/simulation.hpp',
    'src/rsg/simple_commands.cpp',
    'src/rsg/simple_commands.hpp'
]

librsg_src = [
    'src/common/assert.hpp',
    'src/common/message.hpp',
    'src/common/network/tcp_socket.hpp',
    'src/common/network/tcp_socket.cpp',
    'src/librsg/client.cpp',
    'src/librsg/client.hpp'
]

rsg_src += protobuf_generated_src
librsg_src += protobuf_generated_src

rsg_include_dir = include_directories('src/rsg')
rsg = executable('rsg', rsg_src,
    include_directories: rsg_include_dir,
    dependencies: rsg_deps,
    install: true
)

librsg_include_dir = include_directories('src')
librsg = shared_library('rsg', librsg_src,
    include_directories: librsg_include_dir,
    dependencies: librsg_deps,
    install: true,
)

# Install librsg.
install_headers(
    'src/librsg/client.hpp',
    subdir: 'librsg'
)

pkgc = import('pkgconfig')
pkgc.generate(name: 'librsg',
    libraries: librsg,
    version: meson.project_version(),
    requires: [protobuf_dep],
    description: 'Remote SimGrid client library.'
)
