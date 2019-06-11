#include <simgrid/actor.h>
#include <simgrid/s4u.hpp>

#include "simulation.hpp"
#include "../common/assert.hpp"

struct MaestroArgs
{
    rsg::message_queue * to_command;
};

static void useless_actor()
{
    printf("Hello from useless_actor!\n");
    simgrid::s4u::this_actor::sleep_for(1);
}

static void maestro(void * void_args)
{
    // Retrieve arguments, use them then clean memory.
    MaestroArgs * args = static_cast<MaestroArgs*>(void_args);
    rsg::message_queue * to_command = args->to_command;
    delete args;

    // Spawn initial actors.
    simgrid::s4u::Engine * e = simgrid::s4u::Engine::get_instance();
    simgrid::s4u::Actor::create("useless", e->get_all_hosts()[0], useless_actor);

    // Run the simulation.
    printf("Starting SimGrid simulation.\n");
    e->run();
    printf("SimGrid simulation has finished.\n");

    // Tell the command thread that the simulation has finished.
    rsg::InterthreadMessage msg;
    msg.type = rsg::InterthreadMessageType::SIMULATION_FINISHED;
    to_command->push(msg);
}

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command)
{
    // Run a meastro thread.
    auto args = new MaestroArgs;
    args->to_command = to_command;
    SIMIX_set_maestro(maestro, args);

    /* Prepare argc/argv for Engine creation.
       - Changing the maestro thread is only supported for the thread context factory,
         this is why this factory is forced here.
       - Cleaner methods cannot be be used to set SimGrid options,
         as the context factory is managed at engine creation time.
    */
    int argc = 2 + simgrid_options.size();
    char * argv[argc];
    argv[0] = strdup("rsg");
    argv[1] = strdup("--cfg=contexts/factory:thread");
    for (int i = 2; i < argc; i++)
        argv[i] = strdup(simgrid_options[i-2].c_str());
    simgrid::s4u::Engine e(&argc, argv);
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    e.load_platform(platform_file);

    // Become one of the simulated processes (for a very short time).
    // This is required for now (tested with SimGrid-3.22.2).
    RSG_ASSERT(e.get_host_count() > 0,
        "Error: Provided platform file '%s' contains no host", platform_file.c_str());
    sg_actor_attach("temporary", nullptr, e.get_all_hosts()[0], nullptr);

    // Become thread0 again!
    sg_actor_detach();
}
