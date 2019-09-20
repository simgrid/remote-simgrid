{ kapack ? import
    (fetchTarball "https://github.com/oar-team/kapack/archive/master.tar.gz")
  {}
, simgrid ? kapack.simgrid322_2.overrideAttrs (oldAttrs: rec {
    version = "2019-09-20-git";
    rev = "ed970a75d1e3d9460d02481e7b1c3775389eba4f";
    src = kapack.pkgs.fetchurl {
      url = "https://framagit.org/simgrid/simgrid/-/archive/${rev}/simgrid-${rev}.tar.gz";
      sha256 = "1qv055biqi2cvx967vp9zciixw97355isis1npkbvm7bx2ajfqgl";
    };
  })
, doCoverage ? true
, useClang ? false
}:

assert kapack.pkgs.lib.assertMsg (!(doCoverage && useClang)) "Cannot do coverage with clang";

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
