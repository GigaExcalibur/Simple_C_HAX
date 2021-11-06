
# Simple C Hax for FE8U - based on Stan's FE-CHAX

Intended as a simple introduction to writing C hax using the FE8U decompilation.

## how2

### Windows Users: refer to "notes on installing chax.txt"

You need to be on some Linux (Windows users: consider using [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)).

This repository makes use of the git submodule facilities. If you didn't clone the repository using `git clone --recursive`, then you will need to `git submodule update --init --recursive` before being able to build this.

- have [devkitARM](https://devkitpro.org/wiki/Getting_Started) installed, and the `DEVKITARM` env variable set.
- have [python 3](https://www.python.org/) installed and in your path. (the makefile *should* be able to find the correct python executable name. See [`tooks.mk`](./tooks.mk)).
- have built Tools/EventAssembler by running `Tools/EventAssembler/build.sh`. [See the attached README for details](https://github.com/StanHash/EventAssembler/blob/main/README.md).

Once all of that is done, you can navigate to this folder in your shell and do the following:

- `make` or `make hack` to build the ROM (requires `FE8U.gba` in the directory)
- If you want to make a speicifc hack, simply include its main installer from `Main.event` or `Wizardry/Wizardry.event` and `make` again.
- you can `make SomeFile.[o|asm|dmp|lyn.event]` to make a specific file (`asm` files are generated from C).
- run `make clean` to clean build outputs.

hf :)

## General `make`/EA guidelines

- **_Never_ use spaces in filenames/paths.** This breaks `make` horribly. This is one of the main reason I had to modify most of circles tools for them to work with this setup.
- Don't use `#incext`/`#inctext` unless you *really* need to. Use `#include`/`#incbin` with files generated from Makefile rules instead. This speeds up the build process tremendously.

## Stan Appreciation Zone

Stan Appreciation Zone
