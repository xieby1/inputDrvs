# A tool to extract nix inputDrvs

This tiny tool can extract `inputDrvs` from derivations (.drv) file.

## Build

```bash
nix-build ./package.nix
```

Or, import to your nix config, take home-manager for an instance:

```nix
home.packages = [
  (pkgs.callPackage (builtins.fetchTarball {
    url = "https://github.com/xieby1/inputDrvs/archive/74e1cced80a7c5b0c3c9ad0ae91bf782570d1de8.tar.gz";
    sha256 = "11pfvv5bxl1b1w4cjsncy5w5r7y3xyjin2bhbkib2q2q2i8vwk0k";
  }) {})
];
```

## Usage

* run test
  ```bash
  inputDrvs -r $(nix-instantiate ./test.nix)
  ```
* print multiple derivations' inputDrvs.
  ```bash
  inputDrvs -r xxx.drv yyy.drv zzz.drv ...
  ```
* calculate hello's inputDrvs size
  ```bash
  du -chd0 $(inputDrvs -r $(nix-instantiate '<nixpkgs>' -A hello))
  ```

## Why nix does not provide `inputDrvs` in nix expression? E.g. pkgs.hello.inputDrvs.

I do not know.
I think it is good feature to extend nix's management.

## In which case do I need to use `inputDrvs`?

I do not want to push the whole /nix/store into my binary cache,
as there are exprimental/temporary/private files in /nix/store.
I want to fine-grained control what to push into my binary cache.

For an instance, I struggled to build a RISC-V package, which consumes my many times of attempts and modification of the building scripts.
Therefore, there are many intermediate derivations `A`, `B`, `C`, ..., they exist in /nix/store.
After my success of building this package got a derivation `Z`, I want to push `Z` and all its dependencies to binary cache.
The dependencies include the whole RISC-V toolchains, RISC-V .so files and so on.

I have several potential solutions:

### nix.conf's post-build-hook

This could be incomplete.
As some of `Z` dependencies have been built in `A`, `B`, `C`, ... and not all `Z`'s dependencies will trigger post-build-hook.

### Push whole /nix/store

The intermediate derivations `A`, `B`, `C`, ... are also pushed to into binary cache, which waste the storage space.

### Collect dependencies by nix expression

The nix expression like below is also incomplete.

```nix
with pkgs.hello;
  buildInputs ++ depsBuildBuild ++ depsBuildBuildPropagated
  ++ depsBuildTarget ++ depsBuildTargetPropagated ++ depsHostHost
  ++ depsHostHostPropagated ++ depsTargetTarget ++ depsTargetTargetPropagated
  ++ nativeBuildInputs ++ propagatedBuildInputs ++ propagatedNativeBuildInputs
  ++ [stdenv]
```

They are incomplete for 2 reasons:

* These nix variables are only used in `mkDerivatioin`.
  However, not all package are built by `mkDerivatioin`.
  The basic derivation mechanism is `builtins.derivation`.
* Beyond these nix variables, the packages used in any nix variables should be regarded as dependencies.
  For example, [test.nix](./test.nix), the packages in builder script.

### TODO: nix-store command

Quite similar to my demands!

`man nix-store-query`

```bash
Print the build-time dependencies of svn:

$ nix-store --query --requisites $(nix-store --query --deriver $(which svn))
/nix/store/02iizgn86m42q905rddvg4ja975bk2i4-grep-2.5.1.tar.bz2.drv
/nix/store/07a2bzxmzwz5hp58nf03pahrv2ygwgs3-gcc-wrapper.sh
/nix/store/0ma7c9wsbaxahwwl04gbw3fcd806ski4-glibc-2.3.4.drv
... lots of other paths ...
```

## Related information

* [NixOS Discourse: Any way to get a derivation’s inputDrvs from within Nix?](https://discourse.nixos.org/t/any-way-to-get-a-derivations-inputdrvs-from-within-nix/7212)
  * [GitHub: NixOS/nix issues: How to get the build-time dependencies of a package?](https://github.com/NixOS/nix/issues/1245#issuecomment-401642781)
* [Reddit: NixOS: How do derivations know the `inputDrvs`?](https://www.reddit.com/r/NixOS/comments/ooe7yc/how_do_derivations_know_the_inputdrvs/)
