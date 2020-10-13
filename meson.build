project('remote-simgrid', 'cpp',
    version: '0.3.0',
    license: 'LGPL-3.0',
    meson_version: '>=0.42.0',
    default_options: [
        'cpp_std=c++11',
        # Do not use -Wl,--as-needed.
        # librsg has a constructor and its link should always be kept.
        'b_asneeded=false'
    ]
)

# Dependencies
boost_dep = dependency('boost', required: true)
docopt_dep = dependency('docopt', required: true)
protobuf_dep = dependency('protobuf', required : true)
protoc = find_program('protoc', required : true)
simgrid_dep = dependency('simgrid', required: true)
thread_dep = dependency('threads', required: true)

rsg_deps = [
    boost_dep,
    docopt_dep,
    protobuf_dep,
    simgrid_dep,
    thread_dep
]

librsg_deps = [
    protobuf_dep,
    thread_dep
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
    'src/rsg/interthread_messaging.cpp',
    'src/rsg/interthread_messaging.hpp',
    'src/rsg/main.cpp',
    'src/rsg/serve.cpp',
    'src/rsg/serve.hpp',
    'src/rsg/simulation.cpp',
    'src/rsg/simulation.hpp',
    'src/rsg/simple_commands.cpp',
    'src/rsg/simple_commands.hpp',
]

librsg_src = [
    'src/common/assert.hpp',
    'src/common/message.hpp',
    'src/common/network/tcp_socket.hpp',
    'src/common/network/tcp_socket.cpp',
    'src/librsg.hpp',
    'src/librsg/actor.cpp',
    'src/librsg/actor.hpp',
    'src/librsg/comm.cpp',
    'src/librsg/comm.hpp',
    'src/librsg/connection.cpp',
    'src/librsg/connection.hpp',
    'src/librsg/engine.cpp',
    'src/librsg/engine.hpp',
    'src/librsg/host.cpp',
    'src/librsg/host.hpp',
    'src/librsg/mailbox.cpp',
    'src/librsg/mailbox.hpp',
    'src/librsg/mutex.cpp',
    'src/librsg/mutex.hpp',
    'src/librsg/pointers.hpp',
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
    'src/common/assert.hpp',
    'src/librsg/actor.hpp',
    'src/librsg/comm.hpp',
    'src/librsg/connection.hpp',
    'src/librsg/engine.hpp',
    'src/librsg/host.hpp',
    'src/librsg/log.hpp',
    'src/librsg/mailbox.hpp',
    'src/librsg/mutex.hpp',
    'src/librsg/pointers.hpp',
    subdir: 'librsg'
)
install_headers('src/librsg.hpp')

pkgc = import('pkgconfig')
pkgc.generate(name: 'librsg',
    libraries: librsg,
    version: meson.project_version(),
    requires: [protobuf_dep],
    extra_cflags: ['-pthread'],
    description: 'Remote SimGrid client library.'
)

librsg_dep = declare_dependency(
    include_directories: librsg_include_dir,
    link_with: librsg,
    dependencies: librsg_deps,
)

# Test
subdir('tests')
