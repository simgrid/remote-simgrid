# Base image with Nix installed: https://github.com/LnL7/nix-docker
FROM lnl7/nix:1.11.16

# Survival kit
RUN nix-env -i git gnutar gzip gnused coreutils which findutils

# Retrieve kapack packages
RUN git clone https://github.com/oar-team/kapack.git /kapack

# Pre install Remote SimGrid's dependencies
RUN nix-shell /kapack -A remote_simgrid \
              --command "echo Preinstalling dependencies done"

