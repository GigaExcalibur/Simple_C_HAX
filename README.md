
# Simple C Hax for FE8U - based on Stan's FE-CHAX

Intended as a simple introduction to writing C hax using the FE8U decompilation.

## Windows Users: refer to "notes on installing chax.txt"

Installing WSL:
1. Open powershell and `wsl --install`
2. Restart computer when ready
3. A console window opens and you can set your username and password

Navigate to the folder you will be putting the repository in:
1. `cd /mnt/c/`  (replace with /mnt/d/ or whatever drive letter you want)
2. You are now in your C:\ drive - `cd path/to/your/folder` (use forward slashes not backslashes)
3. MAKE SURE THERE ARE NO SPACES IN THE PATH

Clone the Simple_C_HAX repository:
1. `git clone --recursive https://github.com/boviex/Simple_C_HAX.git Simple_C_HAX`

Install devkitpro:
1. `sudo apt update`
2. `sudo apt install gdebi-core`
3a. open a command prompt in windows and `echo %PROCESSOR_ARCHITECTURE%` to see if you have amd64
3b. ASSUMING YOU HAVE AMD64 SYSTEM: `wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb`
4. `sudo gdebi devkitpro-pacman.amd64.deb`
5a. `sudo dkp-pacman -S gba-dev`
5b. Press Enter at the prompt: `Enter a selection (default=all):`
5c. Enter Y at the prompt: `Proceed with installation?`
6. `logout` and then open WSL using "OpenTerminal.bat"

install python3 and pip packages:
1. `sudo apt install python3 python3-pip python-is-python3`
2. `logout` and then run OpenTerminal.bat
3. `pip install pyinstaller`
4. `pip install tmx`

build EventAssembler:
1a. `sudo apt install \
  build-essential cmake re2c \
  ghc cabal-install libghc-vector-dev libghc-juicypixels-dev`
1b. `cabal update`
2a. `wget https://packages.microsoft.com/config/ubuntu/21.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb`
2b. `sudo dpkg -i packages-microsoft-prod.deb`
2c. `rm packages-microsoft-prod.deb`
2d. `sudo apt update; \
  sudo apt install -y apt-transport-https && \
  sudo apt update && \
  sudo apt install -y dotnet-sdk-5.0`
3a. `cd Tools/EventAssembler`
3b. `./build.sh`

Finally, making the hack:
1. OpenTerminal.bat if WSL is not already open
2. `make hack`

## General `make`/EA guidelines

- **_Never_ use spaces in filenames/paths.** This breaks `make` horribly. This is one of the main reason I had to modify most of circles tools for them to work with this setup.
- Don't use `#incext`/`#inctext` unless you *really* need to. Use `#include`/`#incbin` with files generated from Makefile rules instead. This speeds up the build process tremendously.

## Stan Appreciation Zone

Stan Appreciation Zone
