{ stdenv
, nix
, boost
, nlohmann_json
, cli11
}:
stdenv.mkDerivation {
  name = "inputDrvs";
  src = ./.;
  buildInputs = [
    nix
    boost
    nlohmann_json
    cli11
  ];
  makeFlags = [
    "SYSTEM=${stdenv.system}"
    "PREFIX=$(out)"
  ];
}
