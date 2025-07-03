cmake_minimum_required(VERSION 3.10)
project(RedNet VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Engine source files
set(ENGINE_SOURCES
    engine/Node.cpp
    engine/Scanner.cpp
    engine/Exploit.cpp
    engine/Payload.cpp
)

# Main executable
add_executable(RedNet
    main.cpp
    ${ENGINE_SOURCES}
)

# Link libraries
target_link_libraries(RedNet Threads::Threads)

# Set output directory
set_target_properties(RedNet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS RedNet
    RUNTIME DESTINATION bin
)

# Copy headers for development
install(DIRECTORY engine/
    DESTINATION include/engine
    FILES_MATCHING PATTERN "*.h"
)

# Print configuration info
message(STATUS "RedNet Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Add test target (placeholder for future tests)
enable_testing()
add_custom_target(tests
    COMMAND ${CMAKE_COMMAND} -E echo "Tests not implemented yet"
    COMMENT "Running tests..."
)

# Add documentation target
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation generation not implemented yet"
    COMMENT "Generating documentation..."
)

# Add clean target for build artifacts
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build artifacts..."
) 