.\vcpkg.exe install --triplet x64-windows wxwidgets sdl2
.\vcpkg.exe integrate install
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .