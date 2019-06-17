# Make sure tesh is found.
tesh = find_program('tesh', required : true)

# The environment into which the tests are executed.
test_env = environment()
test_env.prepend('PATH', meson.build_root(), meson.build_root() + '/tests')
test_env.prepend('LD_LIBRARY_PATH', meson.build_root())
test_env.set('TEST_DIR', meson.current_source_dir())

# Build some client programs.
executable('client-hello', ['clients/hello.cpp'], dependencies: librsg_dep)
executable('client-hello-manual-connect', ['clients/hello_manual_connect.cpp'], dependencies: librsg_dep)
executable('client-segfault', ['clients/segfault.cpp'], dependencies: librsg_dep)
executable('client-sleep-for', ['clients/sleep_for.cpp'], dependencies: librsg_dep)
executable('client-sleep-until', ['clients/sleep_until.cpp'], dependencies: librsg_dep)
executable('client-actor-whoami', ['clients/actor_whoami.cpp'], dependencies: librsg_dep)
executable('client-actor-create', ['clients/actor_create.cpp'], dependencies: librsg_dep)
executable('client-host-finder', ['clients/host_finder.cpp'], dependencies: librsg_dep)

# Define some tests
test('rsg-cli add-actor', tesh, env: test_env, args: [files('rsg-cli/add-actor.tesh')])
test('rsg-cli kill', tesh, env: test_env, args: [files('rsg-cli/kill.tesh')])
test('rsg-cli start', tesh, env: test_env, args: [files('rsg-cli/start.tesh')])
test('rsg-cli status', tesh, env: test_env, args: [files('rsg-cli/status.tesh')])
test('rsg-cli serve-add-actor-bad-cmd', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-bad-cmd.tesh')])
test('rsg-cli serve-add-actor-start', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-start.tesh')])
test('rsg-cli serve-add-actor-start-manual-connect', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-start-manual-connect.tesh')])
test('rsg-cli serve-add-actor-start-segfault', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-start-segfault.tesh')])
test('rsg-cli serve-kill', tesh, env: test_env, args: [files('rsg-cli/serve-kill.tesh')])
test('actor sleep_for', tesh, env: test_env, args: [files('actor/sleep_for.tesh')])
test('actor sleep_until', tesh, env: test_env, args: [files('actor/sleep_until.tesh')])
#test('actor create', tesh, env: test_env, args: [files('actor/create.tesh')])
test('actor whoami', tesh, env: test_env, args: [files('actor/whoami.tesh')])
test('host finder', tesh, env: test_env, args: [files('host/finder.tesh')])
