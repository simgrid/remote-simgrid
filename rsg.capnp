@0xfa4cc2dce10de2cd;

struct AddActorCommand {
    actorName @0 :Text = "actor";
    hostName @1 :Text;
}

struct Command {
  # This is the first message sent by any client to the RSG server.
  type :union {
    addActor @0 :AddActorCommand;
    start @1 :Void;
    kill @2 :Void;
    status @3 :Void;
  }
}

# struct Actor {
#   id @0 :UInt64;
# }

# struct Host {
#   id @0 :UInt64;
# }



