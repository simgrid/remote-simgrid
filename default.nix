{ kapack ? import
    (fetchTarball "https://github.com/oar-team/kapack/archive/master.tar.gz")
  {}
, simgrid ? kapack.simgrid322_2
}:

let
  pkgs = kapack.pkgs;

  jobs = rec {
    remote_simgrid = pkgs.stdenv.mkDerivation rec {
      pname = "remote-simgrid";
      version = "0.3.0-git";

      src = ./.;
      nativeBuildInputs = [ pkgs.meson pkgs.pkgconfig pkgs.ninja ];
      buildInputs = [ simgrid kapack.docopt_cpp pkgs.boost pkgs.protobuf ];

      preConfigure = "rm -rf build";

      doCheck = true;
    };
  };
in
  jobs.remote_simgrid
