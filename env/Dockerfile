FROM nixos/nix:2.2.1
RUN nix-channel --remove nixpkgs \
 && nix-channel --add https://nixos.org/channels/nixos-19.03 nixpkgs \
 && nix-channel --update \
 && nix-env -f '<nixpkgs>' -iA \
      bash \
      coreutils \
      curl \
      findutils \
      gitMinimal \
      glibc \
      gnugrep \
      gnused \
      gnutar \
      gzip \
      netcat-gnu \
      openssh \
      psmisc \
      rsync \
      which \
      xz \
 && nix-env -iA cachix -f https://cachix.org/api/v1/install \
 && cachix use simgrid \
 && nix-collect-garbage -d \
 && ln -s $(realpath $(which bash)) /bin/bash
