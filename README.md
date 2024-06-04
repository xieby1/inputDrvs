# A tool to extract nix inputDrvs

This tiny tool can extract `inputDrvs` from derivations (.drv) file.

## Build

```bash
nix-build ./package.nix
# test
nix-instantiate
./result/bin/inputDrvs /nix/store/replace-with-instantiated-drv-xx-test-inputDrvs.drv
```

## Usage

```bash
inputDrvs xxx.drv yyy.drv zzz.drv ...
```

It will print these derivations' direct inputDrvs.

```bash
inputDrvs -r xxx.drv yyy.drv zzz.drv ...
```

It will recursively print these derivations inputDrvs.

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

## Related information

* [NixOS Discourse: Any way to get a derivation’s inputDrvs from within Nix?](https://discourse.nixos.org/t/any-way-to-get-a-derivations-inputdrvs-from-within-nix/7212)
  * [GitHub: NixOS/nix issues: How to get the build-time dependencies of a package?](https://github.com/NixOS/nix/issues/1245#issuecomment-401642781)
* [Reddit: NixOS: How do derivations know the `inputDrvs`?](https://www.reddit.com/r/NixOS/comments/ooe7yc/how_do_derivations_know_the_inputdrvs/)
