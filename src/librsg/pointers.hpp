#include <memory>

namespace rsg
{
    class Actor;
    class Comm;
    class Host;
    class Mailbox;

    typedef std::shared_ptr<Actor> ActorPtr;
    typedef std::shared_ptr<Comm> CommPtr;
    typedef std::shared_ptr<Host> HostPtr;
    typedef std::shared_ptr<Mailbox> MailboxPtr;
};
