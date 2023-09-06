# Emley
The first L1 module for Adastral (used for versioning, patching & verifying files)

To clone, use the following command;
```
git clone https://github.com/AdastralGroup/emley.git --recurse
```

## Setup Environment (Windows)
Install [Visual Studio 2022 via the Visual Studio Installer](https://visualstudio.microsoft.com/downloads/) and make sure the following workloads are enabled;
- `Desktop Development with C++`
- `Linux and embedded development with C++`
![screenshot showing what workloads to enable](https://xb.redfur.cloud/tOpi9/FEFUneKU67.png/raw)

Once you've done that, open up "Developer Command Prompt for Visual Studio 2022" and change directory to where you have Emley cloned.

After you've changed to where Emley was cloned, run `cmake -b build -G 'Visual Studio 17 2022'` to initialize the Visual Studio solution.

You can open the `.sln` file in `build/adastral.sln`. Make sure that the `adastral` project is selected as the startup project and you should be able to build/run it.

## Setup Environment (Debian)
Install dependencies for Debian Bookworm (12)
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    g++ \
    libzstd-dev \
    libarchive-dev
```

Create CMake stuff
```bash
cmake .
```

Build Project
```bash
cmake --build .
```