cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(GEODE_DISABLE_PRECOMPILED_HEADERS ON)

project(ManagerViewer VERSION 1.1.2)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/ModuleManager.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Fix annoying compiler errors with Dear ImGui
if (WIN32 AND NOT MSVC)
    # Use builtin offsetof and disable SSE
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_USE_BUILTIN_OFFSETOF IMGUI_DISABLE_SSE)
endif()

set(IMGUI_VERSION "v1.91.9b")
CPMAddPackage("gh:matcool/gd-imgui-cocos#1ae8317")

target_link_libraries(${PROJECT_NAME} imgui-cocos)

setup_geode_mod(${PROJECT_NAME})
