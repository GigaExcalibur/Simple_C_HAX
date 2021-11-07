
# Simple C Hax for FE8U - based on Stan's FE-CHAX

Intended as a simple introduction to writing C hax using the FE8U decompilation.

## Setup Guide (Windows Users)

### Environment Setup

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

Install devkitpro for WSL:
1. `sudo apt update`
2. `sudo apt install gdebi-core`
3. open a command prompt in windows and `echo %PROCESSOR_ARCHITECTURE%` to see if you have amd64
4. ASSUMING YOU HAVE AMD64 SYSTEM: `wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb`
5. `sudo gdebi devkitpro-pacman.amd64.deb`
6. `sudo dkp-pacman -S gba-dev`
7. Press Enter at the prompt: `Enter a selection (default=all):`
8. Enter Y at the prompt: `Proceed with installation?`
9. `logout` and then open WSL using "OpenTerminal.bat" (this should open WSL with the correct folder already but if not, use `cd` to navigate to the Simple_C_HAX folder)

install python3 and pip packages for WSL:
1. `sudo apt install python3 python3-pip python-is-python3`
2. `logout` and then run OpenTerminal.bat
3. `pip install pyinstaller`
4. `pip install tmx`

build EventAssembler:
1. `sudo apt install \build-essential cmake re2c \ghc cabal-install libghc-vector-dev libghc-juicypixels-dev`
2. `cabal update`
3. `wget https://packages.microsoft.com/config/ubuntu/21.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb`
4. `sudo dpkg -i packages-microsoft-prod.deb`
5. `rm packages-microsoft-prod.deb`
6. `sudo apt update; \sudo apt install -y apt-transport-https && \sudo apt update && \sudo apt install -y dotnet-sdk-5.0`
7. `sudo apt install dos2unix`
8. `find . -type f -print0 | xargs -0 dos2unix` (this step fixes windows line endings for all files in all subfolders so make sure you run it from the Simple_C_HAX folder)
9. `cd Tools/EventAssembler`
10. `./build.sh`
11. Check inside Tools/EventAssembler/Tools - there should be "Tool Helpers.txt", but if not get it from https://github.com/StanHash/EventAssembler/blob/8aa0709a3be725f9cb2174f08763b59b4f937560/.Sources/Tool%20Helpers.txt

Finally, making the hack:
1. Put a clean FE8U rom named "FE8U.gba" in the root folder
2. Optional: put a symbol file named "FE8U.sym" in the root folder (here is one, just rename it to FE8U.sym https://cdn.discordapp.com/attachments/179027738454261760/906677647222919188/fe8u-20211009.sym)
3. OpenTerminal.bat if WSL is not already open
4. `make hack`

### Making C Hax
1. Find the routine you want to replace from https://github.com/FireEmblemUniverse/fireemblem8u
2. In the Wizardry/Easy_C folder, open "decomp.c"
3. Paste the relevant code into decomp.c and make any edits you need
4. Go back up to the Simple_C_HAX folder and `make hack`

## General `make`/EA guidelines

- **_Never_ use spaces in filenames/paths.** This breaks `make` horribly. This is one of the main reason I had to modify most of circles tools for them to work with this setup.
- Don't use `#incext`/`#inctext` unless you *really* need to. Use `#include`/`#incbin` with files generated from Makefile rules instead. This speeds up the build process tremendously.

## Stan Appreciation Zone

Stan Appreciation Zone
