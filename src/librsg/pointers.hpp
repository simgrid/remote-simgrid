#include <memory>

namespace rsg
{
    class Actor;
    class Comm;
    class ConditionVariable;
    class Host;
    class Mailbox;
    class Mutex;

    typedef std::shared_ptr<Actor> ActorPtr;
    typedef std::shared_ptr<Comm> CommPtr;
    typedef std::shared_ptr<ConditionVariable> ConditionVariablePtr;
    typedef std::shared_ptr<Host> HostPtr;
    typedef std::shared_ptr<Mailbox> MailboxPtr;
    typedef std::shared_ptr<Mutex> MutexPtr;
};
