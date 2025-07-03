cmake_minimum_required(VERSION 3.10)
project(RedNet VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ferror-limit=2000 -Wno-format-security")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# macOS specific flags
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
    add_definitions(-DGL_SILENCE_DEPRECATION)
    # Suppress GLFW null character warnings and UTF-8 errors
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-null-character -Wno-invalid-source-encoding")
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Include GLFW CFLAGS
include_directories(${GLFW_INCLUDE_DIRS})

# Manual ImGui setup to avoid corruption issues
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
if(NOT EXISTS "${IMGUI_DIR}")
    message(STATUS "Downloading ImGui...")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external")
    execute_process(
        COMMAND git clone --depth 1 --branch v1.90.4 https://github.com/ocornut/imgui.git "${IMGUI_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external"
        RESULT_VARIABLE IMGUI_DOWNLOAD_RESULT
    )
    if(NOT IMGUI_DOWNLOAD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to download ImGui")
    endif()
endif()

# Add ImGui source files
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/shell
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${IMGUI_DIR}
    ${GLFW_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    main.cpp
    ai/AIAgent.cpp
    ai/AIAgentManager.cpp
    engine/Exploit.cpp
    engine/Node.cpp
    engine/Payload.cpp
    engine/Scanner.cpp
    network/NetworkGraph.cpp
    network/Packet.cpp
    network/TopologyGenerator.cpp
    shell/Shell.cpp
    ui/AdvancedVisualizer.cpp
    ui/Visualizer.cpp
    ui/desktop/ImGuiDashboard.cpp
    utils/Config.cpp
    ${IMGUI_SOURCES}
)

# Create executable
add_executable(RedNet ${SOURCES})

# Link libraries (with GUI support)
target_link_libraries(RedNet PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

# macOS app bundle configuration
if(APPLE)
    set_target_properties(RedNet PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist"
        MACOSX_BUNDLE_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/RedNet.icns"
    )
    
    # Install the app bundle
    install(TARGETS RedNet
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
else()
    # Install executable on other platforms
    install(TARGETS RedNet DESTINATION bin)
endif()

# Copy headers for development
install(DIRECTORY engine/
    DESTINATION include/engine
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY network/
    DESTINATION include/network
    FILES_MATCHING PATTERN "*.h"
)

# Ensure fonts directory exists and copy FontAwesome font (if present)
install(DIRECTORY fonts/ DESTINATION ${CMAKE_BINARY_DIR}/fonts)

# Copy fonts into the bundle
add_custom_command(TARGET RedNet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:RedNet>/../Resources/fonts
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