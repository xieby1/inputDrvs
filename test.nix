let
  pkgs = import <nixpkgs> {};
  name = "test-inputDrvs";
in builtins.derivation {
  inherit name;
  system = builtins.currentSystem;
  builder = pkgs.writeShellScript "builder" ''
    echo ${name} > $out
    echo "${pkgs.hello}" >> $out
  '';
}
