{ stdenv
, nix
, boost
, nlohmann_json
}:
stdenv.mkDerivation {
  name = "inputDrvs";
  src = ./.;
  buildInputs = [
    nix
    boost
    nlohmann_json
  ];
  makeFlags = [
    "PREFIX=$(out)"
  ];
}
