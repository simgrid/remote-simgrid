# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog][changelog]
and this project adheres to [Semantic Versioning][semver].

As the project is currently unstable, Remote Simgrid's API is not defined yet.

[//]: =========================================================================
## [Unreleased]
### Added
- *rsg_server* can now be used with the `--background` flag, which executes
  the server in background and terminate immediately.
- *rsg_server* can now be used with the `--server-only` flag, which prevents
  the automatic execution of the clients.
- *rsg_server*'s CLI now has options (and no longer has positional arguments).
- Servers are now launched in parallel.  
  Previously, clients needed to connect in a sequential fashion to rsg_server.
- *rsg_server* now runs a status service, which notably allows to retrieve
  information about the current client connections.  
  Requests are formatted in plain text and follow the common ZeroMQ
  request-reply pattern.
  Current supported requests are `status all` and `status waiting_init`.  
  This service uses TCP, and is bound on port 4242 by default.
  Port can be changed via *rsg_server*'s command-line option `--status-port`.

[//]: =========================================================================
## 0.1.0 - 2018-03-12
Initial release.

[//]: =========================================================================
[changelog]: http://keepachangelog.com/en/1.0.0/
[semver]: http://semver.org/spec/v2.0.0.html

[Unreleased]: https://gitlab.inria.fr/batsim/batsched/compare/v0.1.0...master
