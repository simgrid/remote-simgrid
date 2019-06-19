{ kapack ? import
    (fetchTarball "https://github.com/oar-team/kapack/archive/master.tar.gz")
  {}
, simgrid ? kapack.simgrid322_2.overrideAttrs (oldAttrs: rec {
    version = "3.22.2-tesh-fix";
    rev = "57d26848355f0928264339d28412d8b87196d668";
    src = kapack.pkgs.fetchurl {
      url = "https://framagit.org/simgrid/simgrid/-/archive/${rev}/simgrid-${rev}.tar.gz";
      sha256 = "1ya19x05dzjyn58h4055aq3dai2w8f94wgkb5w38ijf1qaj2607h";
    };
  })
, doCoverage ? true
, useClang ? false
}:

let
  pkgs = kapack.pkgs;
  stdenv = if useClang then pkgs.clangStdenv else pkgs.stdenv;

  jobs = rec {
    meson = pkgs.meson.override { inherit stdenv; };
    remote_simgrid = stdenv.mkDerivation rec {
      pname = "remote-simgrid";
      version = "0.3.0-git";

      src = ./.;
      nativeBuildInputs = [ meson pkgs.pkgconfig pkgs.ninja ]
        ++ pkgs.lib.optional doCoverage [ kapack.gcovr ];
      buildInputs = [ simgrid kapack.docopt_cpp pkgs.boost pkgs.protobuf ];

      preConfigure = "rm -rf build";
      mesonFlags = []
        ++ pkgs.lib.optional doCoverage [ "-Db_coverage=true" ];
      postCheck = pkgs.lib.optionalString doCoverage ''
        mkdir -p ./cov/report-html
        gcovr . -e '.*rsg\.pb\..*' -o ./cov/report.txt 2>/dev/null
        gcovr . -e '.*rsg\.pb\..*' -o ./cov/report-html/index.html --html-details 2>/dev/null
      '';
      postInstall = pkgs.lib.optionalString doCoverage ''
        mkdir -p $out/cov/rsg@exe $out/cov/rsg@sha
        cp rsg@exe/*.gc{no,da} $out/cov/rsg@exe/
        cp rsg@sha/*.gc{no,da} $out/cov/rsg@sha/
        cp -r ./cov/* $out/cov
      '';

      doCheck = true;
    };
  };
in
  jobs.remote_simgrid
