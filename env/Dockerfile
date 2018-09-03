# Base image with Nix installed: https://github.com/LnL7/nix-docker
FROM lnl7/nix:2018-04-17

# Survival kit
RUN nix-env -i git gnutar gzip gnused coreutils which findutils

# Network setup (mainly for cachix)
RUN nix-env -i iana-etc
RUN export IANA_PATH=$(nix-env -q --out-path | \
                       sed -n -E 'sWiana-etc-\S+\s+(.*)W\1Wp'); \
    cp ${IANA_PATH}/etc/protocols /etc/; \
    cp ${IANA_PATH}/etc/services /etc/

RUN nix-env -i nss-cacert
RUN export CACERT_PATH=$(nix-env -q --out-path | \
                       sed -n -E 'sWnss-cacert-\S+\s+(.*)W\1Wp'); \
    cp -r ${CACERT_PATH}/etc/ssl /etc/

# Cachix setup
RUN nix-env -iA cachix -f https://github.com/NixOS/nixpkgs/tarball/1d4de0d552ae9aa66a5b8dee5fb0650a4372d148
ENV USER=root
RUN cachix use simgrid

# Retrieve kapack packages
RUN git clone https://github.com/oar-team/kapack.git /kapack

# Pre install Remote SimGrid's dependencies
RUN nix-shell /kapack -A remote_simgrid \
              --command "echo Preinstalling dependencies done"

# Pre-build some other projects (also using this container for CI)
RUN git clone https://framagit.org/simgrid/openmpi-rsg-plugins.git /openmpi-rsg-plugins
RUN nix-build /openmpi-rsg-plugins -A openmpi_rsg
