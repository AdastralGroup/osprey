Building ~~should be relatively straightforward~~ is a somewhat arcane process. As of now, there's nothing currently hooking into the GDExtension
bindings that'd work, so building is just an exercise in filesize and checking nothing's exploded.

Note that libcurl is required currently to build as well.


# ON WINDOWS:

install nlohmann json and libcurl from vcpkg (MAKE SURE THEY'RE X64)

make a build folder, ninja probably works if you're 100% sure it's using c++20 but use vs otherwise

``cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/Users/win/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug``

open it, CHANGE THE BUILD SETTING TO C++20. THIS IS IMPORTANT

build. it should... build if everything's gone to plan.