cmake_minimum_required(VERSION 3.24)
project(RAP)

set (CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(wxWidgets REQUIRED COMPONENTS net core base media)
find_package(SDL2 REQUIRED)
include(${wxWidgets_USE_FILE})

# Includes
include_directories("include/" ${SDL2_INCLUDE_DIR})

# Unix flags
if (UNIX)
    set(COMMON_FLAGS "-Wall -O3 -g")

    # Colour output with Ninja
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(COMMON_FLAGS "${COMMON_FLAGS} -fdiagnostics-color=always")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(COMMON_FLAGS "${COMMON_FLAGS} -fcolor-diagnostics")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -std=c++20")
endif (UNIX)

# Windows flags
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

# Build
add_executable(${PROJECT_NAME} ${SOURCES}
    src/main.cpp
    src/app.cpp
    src/playlist_window.cpp
    src/file_browser.cpp
    src/audio_stream.cpp
    src/audio_file.cpp
    src/playlist.cpp
    src/start_window.cpp
    src/play_window.cpp
    src/fft.cpp
)

# Link
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} SDL2)
