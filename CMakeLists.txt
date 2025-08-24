cmake_minimum_required(VERSION 3.15)
project(NWDAF VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Debug flags
set(CMAKE_BUILD_TYPE Debug)

# For GCC/Clang: include debug symbols (-g) and disable optimizations (-O0)
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer")

set(CMAKE_LINKER_FLAGS_DEBUG "-fsanitize=address,undefined")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/utility/
    ${PROJECT_SOURCE_DIR}/include/3rdParty/
    ${PROJECT_SOURCE_DIR}/include/collection/
    ${PROJECT_SOURCE_DIR}/include/Database/
    /usr/include/postgresql       # PostgreSQL headers
)

# Only include .cpp source files
file(GLOB NWDAF_SOURCES
    ${PROJECT_SOURCE_DIR}/source/*.cpp
    ${PROJECT_SOURCE_DIR}/source/utility/*.cpp
    ${PROJECT_SOURCE_DIR}/source/Database/*.cpp
)

add_executable(NWDAF ${NWDAF_SOURCES})


# ----------------------------
# PostgreSQL (libpq) support
# ----------------------------
if(MSVC)

else()
find_package(PostgreSQL REQUIRED)

if(PostgreSQL_FOUND)
    message(STATUS "PostgreSQL include dirs: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(NWDAF PRIVATE ${PostgreSQL_LIBRARIES})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install libpq-dev.")
endif()
endif()

# Thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(NWDAF PRIVATE Threads::Threads)

# Compiler warnings
if(MSVC)
    target_compile_options(NWDAF PRIVATE /W4 /permissive- /bigobj)
else()
    target_compile_options(NWDAF PRIVATE -Wall -Wextra -Wpedantic -pthread)
endif()

# Output directory
set_target_properties(NWDAF PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# Clear build artifacts
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${PROJECT_BINARY_DIR}/*
    COMMENT "Clearing all build artifacts..."
)
