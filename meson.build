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
    'src/rsg/main.cpp',
    'src/rsg/serve.cpp',
    'src/rsg/serve.hpp',
    'src/rsg/simple_commands.cpp',
    'src/rsg/simple_commands.hpp'
]
rsg_src += protobuf_generated_src

rsg_include_dir = include_directories('src/rsg')

rsg = executable('rsg', rsg_src,
    include_directories: rsg_include_dir,
    dependencies: rsg_deps,
    install: true
)
