{ kapack ? import
    (fetchTarball "https://github.com/oar-team/kapack/archive/master.tar.gz")
  {}
, simgrid ? kapack.simgrid322_2
, doCoverage ? true
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

      mesonFlags = []
        ++ pkgs.lib.optional doCoverage [ "-Db_coverage=true" ];
      postInstall = pkgs.lib.optionalString doCoverage ''
        mkdir -p $out/cov/rsg@exe $out/cov/rsg@sha
        cp rsg@exe/*.gc{no,da} $out/cov/rsg@exe/
        cp rsg@sha/*.gc{no,da} $out/cov/rsg@sha/
      '';

      preConfigure = "rm -rf build";

      doCheck = true;
    };
  };
in
  jobs.remote_simgrid
