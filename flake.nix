{
  description = "TermiType 🚀: A terminal-based typing test application built with C++";

  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = { self, nixpkgs }:
    let
      pkgs = import nixpkgs { system = "x86_64-linux"; };

      BIN_FILE_NAME = "confix";
    in
    {
      packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
        pname = "confix";
        version = "v0.1.2";

        src = ./.;

        buildInputs = [ pkgs.gcc pkgs.gnumake pkgs.argparse pkgs.jsoncpp ];
        buildPhase = "make";
        installPhase = ''
          mkdir -p $out/bin
          cp ${BIN_FILE_NAME} $out/bin/
        '';

        cleanPhase = "make clean";
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = [ pkgs.gcc pkgs.gnumake pkgs.argparse pkgs.jsoncpp ];
      };
    };
}
