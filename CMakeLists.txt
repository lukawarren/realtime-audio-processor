cmake_minimum_required(VERSION 3.24)
project(RAP)

set (CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(SDL2 REQUIRED)
if (NOT WIN32)
    find_package(wxWidgets REQUIRED)
    include(${wxWidgets_USE_FILE})
else()
    find_package(wxWidgets CONFIG REQUIRED)
endif()

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

file(GLOB SOURCE_FILES "include/*/*.h" "include/*.h" "src/*/*.cpp" "src/*.cpp")

# Windows flags
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    set(SOURCE_FILES ${SOURCE_FILES} "dpi.manifest")
endif()

# Build
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link
target_link_libraries(
    ${PROJECT_NAME}
    $<TARGET_NAME_IF_EXISTS:wx::core>
    $<TARGET_NAME_IF_EXISTS:wx::base>
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
    $<TARGET_NAME_IF_EXISTS:SDL2>
)
if (NOT WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        ${wxWidgets_LIBRARIES}
    )
endif()
