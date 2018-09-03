let
  pkgs = import
    ( fetchTarball "https://github.com/NixOS/nixpkgs/archive/18.03.tar.gz") {};
  kapack = import
    ( fetchTarball "https://github.com/oar-team/kapack/archive/master.tar.gz")
    # A local version can also be used:
    #~/kapack
  { inherit pkgs; };
in

kapack.remote_simgrid_dev
