## Note:
We have CI set up for Winter, so you should be able to download the latest libraries without needing to build.


## Building
Building should be relatively straightforward, libcurl is needed as a dependency - all other deps are handled by cmake.

However, do add the following cmake flags when configuring:

``-DGODOT=1 -DGODOT_GDEXTENSION_DIR="gdextension" -DGODOT_CPP_SYSTEM_HEADERS=ON``

These enable the Godot bindings extension - set ``-DGODOT=0`` to build the headless module instead.

# ON WINDOWS:

Note that only MSVC seems to work, though you can use MSBuild or Ninja. Also seemingly only Debug build types work.

Fish out the .dll and associated libraries (libcurl and zlib generally) and put them in the Belmont repository in bin/.