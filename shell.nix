let
  pkgs = import
    ( fetchTarball "https://github.com/NixOS/nixpkgs/archive/18.03.tar.gz") {};
  dmpkgs = import
    ( fetchTarball "https://gitlab.inria.fr/vreis/datamove-nix/repository/master/archive.tar.gz")
    # A local version can also be used:
    #~/datamovepkgs
  { inherit pkgs; };
in

dmpkgs.remote_simgrid_dev
