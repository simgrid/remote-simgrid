# Make sure tesh is found.
tesh = find_program('tesh', required : true)

# The environment into which the tests are executed.
test_env = environment()
test_env.prepend('PATH', meson.build_root(), meson.build_root() + '/tests')
test_env.prepend('LD_LIBRARY_PATH', meson.build_root())
test_env.set('TEST_DIR', meson.current_source_dir())

# Build some client programs.
executable('client-hello', ['clients/hello.cpp'], dependencies: librsg_dep)

# Define some tests
test('rsg-cli add-actor', tesh, env: test_env, args: [files('rsg-cli/add-actor.tesh')])
test('rsg-cli kill', tesh, env: test_env, args: [files('rsg-cli/kill.tesh')])
test('rsg-cli start', tesh, env: test_env, args: [files('rsg-cli/start.tesh')])
test('rsg-cli status', tesh, env: test_env, args: [files('rsg-cli/status.tesh')])
test('rsg-cli serve-add-actor-bad-cmd', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-bad-cmd.tesh')])
test('rsg-cli serve-add-actor-start', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-start.tesh')])
test('rsg-cli serve-kill', tesh, env: test_env, args: [files('rsg-cli/serve-kill.tesh')])
