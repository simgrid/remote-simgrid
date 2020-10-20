{ kapack ? import
    (fetchTarball "https://github.com/oar-team/nur-kapack/archive/master.tar.gz")
  {}
, simgrid ? kapack.simgrid-325
, doCoverage ? true
, coverageCoveralls ? false
, coverageHtml ? false
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

      preConfigure = "rm -rf build cov";
      mesonBuildType = "debug";
      mesonFlags = []
        ++ pkgs.lib.optional doCoverage [ "-Db_coverage=true" ];
      preCheck = pkgs.lib.optionalString doCoverage ''
        mkdir ../cov
        cp rsg@exe/*.gcno ../cov/
        cp rsg@sha/*.gcno ../cov/
        export GCOV_PREFIX=$(realpath ../cov)
        export GCOV_PREFIX_STRIP=5
        export GCOV_ERROR_FILE=/dev/null # do not break logs with gcov messages
      '';
      postCheck = pkgs.lib.optionalString doCoverage ''
        cd ../cov
        gcov -p *.gcno 1>/dev/null 2>&1
        mkdir report
      '' + pkgs.lib.optionalString coverageHtml ''
        mkdir -p report/html
      '' + ''
        gcovr -g -k -r .. --filter '\.\./src/' --exclude '.*tcp_socket.*' \
          --txt report/file-summary.txt \
          --csv report/file-summary.csv \
          --json-summary report/file-summary.json \
      '' + pkgs.lib.optionalString coverageCoveralls ''
          --coveralls report/coveralls.json \
      '' + pkgs.lib.optionalString coverageHtml ''
          --html-details report/html/index.html \
      '' + ''
          --print-summary
          cd -
      '';
      postInstall = pkgs.lib.optionalString doCoverage ''
        cp -r ../cov/report $out/cov-report
      '';

      doCheck = true;
    };
  };
in
  jobs.remote_simgrid
