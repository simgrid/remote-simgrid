# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog][changelog]
and this project adheres to [Semantic Versioning][semver].

As the project is currently unstable, Remote Simgrid's API is not defined yet.

[//]: =========================================================================
## [Unreleased]
### Added
- *rsg_server* can now be used with the `--server-only` flag, which prevents
  the automatic execution of the clients.
- *rsg_server*'s CLI now supports options
  (parameters no longer need to be positional arguments).
- Servers are now launched in parallel.  
  Previously, clients needed to connect in a sequential fashion to rsg_server.

[//]: =========================================================================
## 0.1.0 - 2018-03-12
Initial release.

[//]: =========================================================================
[changelog]: http://keepachangelog.com/en/1.0.0/
[semver]: http://semver.org/spec/v2.0.0.html

[Unreleased]: https://gitlab.inria.fr/batsim/batsched/compare/v0.1.0...master
