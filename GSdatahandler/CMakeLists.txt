cmake_minimum_required(VERSION 3.18)
project(GSdatahandler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LINKER_FLAGS "--add-stdcall-alias")
add_library(GSdatahandler SHARED
        src/compression.cpp
        src/dllmain.cpp
        src/strings.cpp
        src/spacemanager.cpp
)

target_include_directories(GSdatahandler
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        )
