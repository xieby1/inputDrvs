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
    "NIX_SRC=${nix.src}"
    "NIX_OUT=${nix.out}"
    "PREFIX=$(out)"
  ];
}
