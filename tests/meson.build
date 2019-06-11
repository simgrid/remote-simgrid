tesh = find_program('tesh', required : true)
test_env = environment()
test_env.prepend('PATH', meson.build_root())
test_env.set('TEST_DIR', meson.current_source_dir())

test('rsg-cli kill', tesh, env: test_env, args: [files('rsg-cli/kill.tesh')])
test('rsg-cli serve-add-actor-bad-cmd', tesh, env: test_env, args: [files('rsg-cli/serve-add-actor-bad-cmd.tesh')])
test('rsg-cli serve-kill', tesh, env: test_env, args: [files('rsg-cli/serve-kill.tesh')])
