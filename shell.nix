with (import <nixpkgs> { });

mkShell {
  buildInputs = with pkgs; [
    # build essentials
    gcc
    gnumake

    # dependencies
    argparse
    jsoncpp
  ];
}
