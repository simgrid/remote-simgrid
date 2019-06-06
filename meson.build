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
sfml_network_dep = dependency('sfml-network', required: true)
simgrid_dep = dependency('simgrid', required: true)

rsg_deps = [
    boost_dep,
    docopt_dep,
    protobuf_dep,
    sfml_network_dep,
    simgrid_dep
]

# librsg_deps = [
#     protobuf_dep,
#     sfml_network_dep
# ]

# Source files
rsg_src = [
    'src/common/assert.hpp',
    'src/common/message.hpp',
    'src/common/protobuf/rsg.pb.cc',
    'src/common/protobuf/rsg.pb.h',
    'src/rsg/main.cpp',
    'src/rsg/serve.cpp',
    'src/rsg/serve.hpp',
    'src/rsg/simple_commands.cpp',
    'src/rsg/simple_commands.hpp'
]

rsg_include_dir = include_directories('src/rsg')

rsg = executable('rsg', rsg_src,
    include_directories: rsg_include_dir,
    dependencies: rsg_deps,
    install: true
)
