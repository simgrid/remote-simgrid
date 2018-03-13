# Base image with Nix installed: https://github.com/LnL7/nix-docker
FROM lnl7/nix:1.11.16

# Survival kit
RUN nix-env -i git gnutar gzip

# Retrieve datamove nix packages
RUN git clone https://gitlab.inria.fr/vreis/datamove-nix.git /datamovepkgs

# Pre install Remote SimGrid's dependencies
RUN nix-shell /datamovepkgs -A remote_simgrid \
              --command "echo Preinstalling dependencies done"

# Annex programs
RUN nix-env -i findutils coreutils
