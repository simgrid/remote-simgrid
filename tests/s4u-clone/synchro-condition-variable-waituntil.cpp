// From http://www.cplusplus.com/reference/condition_variable/condition_variable/notify_all/
#include <condition_variable>
#include <mutex>
#include <string>

#include <librsg.hpp>

rsg::MutexPtr mutex = nullptr;
rsg::ConditionVariablePtr cv = nullptr;
bool ready = false;

void competitor(void * arg)
{
    auto my_id = (int *)arg;
    RSG_INFO("Sleeping for %d s", *my_id);
    rsg::this_actor::sleep_for(*my_id);

    {
        std::unique_lock<rsg::Mutex> lock(*mutex);
        while (!ready) {
            RSG_INFO("About to call wait_until...");
            double now = rsg::Engine::get_clock();
            if (cv->wait_until(lock, now+0.25*(*my_id+1)) == std::cv_status::timeout)
                RSG_INFO("Out of wait_until: timeout reached :(");
            else
                RSG_INFO("Out of wait_until: YAY notified!");
        }

        RSG_INFO("Running! id=%d", *my_id);
    }
    delete my_id;
}

void go(void *)
{
    rsg::this_actor::sleep_for(2);

    {
        std::unique_lock<rsg::Mutex> lock(*mutex);
        ready = true;
        RSG_INFO("Go go go!");
        cv->notify_all();
    }
}

int main()
{
    mutex = rsg::Mutex::create();
    cv = rsg::ConditionVariable::create();

    auto actor = rsg::Actor::self();
    auto host = actor->get_host();

    rsg::Actor::create("go", host, go, nullptr);
    for (int i = 0; i < 4; i++)
        rsg::Actor::create("competitor", host, competitor, (void*)new int(i));

    // This sleep is there so that main does not end before other functions.
    // This is to avoid mutex/condvar memory dealloc.
    rsg::this_actor::sleep_for(5);

    return 0;
}
