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

# ImGui (Dear ImGui) for desktop GUI
include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.4
)
FetchContent_MakeAvailable(imgui)

# ImGui requires OpenGL and GLFW (for window/input)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${imgui_SOURCE_DIR} ${GLFW_INCLUDE_DIRS})

# Add ImGui source files directly
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    external/imgui/misc/stb_image_impl.cpp
)

# Engine source files
set(ENGINE_SOURCES
    engine/Node.cpp
    engine/Scanner.cpp
    engine/Exploit.cpp
    engine/Payload.cpp
)

# Network source files
set(NETWORK_SOURCES
    network/NetworkGraph.cpp
    network/TopologyGenerator.cpp
)

# Shell source files
set(SHELL_SOURCES
    shell/Shell.cpp
)

# UI source files (with ImGui)
set(UI_SOURCES
    ${IMGUI_SOURCES}
    ui/desktop/ImGuiDashboard.cpp
    ui/AdvancedVisualizer.cpp
)

# AI source files
set(AI_SOURCES
    ai/AIAgentManager.cpp
)

# Utils source files
set(UTILS_SOURCES
    utils/Config.cpp
    utils/Logger.cpp
)

# Missions source files
set(MISSIONS_SOURCES
    missions/Mission.cpp
    missions/mission_launcher.cpp
)

# Main executable
set(MACOSX_BUNDLE_ICON_FILE RedNet.icns)
set(MACOSX_BUNDLE_BUNDLE_NAME "RedNet")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.RedNet")
set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/Info.plist)

set(APP_ICON ${CMAKE_SOURCE_DIR}/resources/RedNet.icns)

add_executable(RedNet MACOSX_BUNDLE
    main.cpp
    ${ENGINE_SOURCES}
    ${NETWORK_SOURCES}
    ${SHELL_SOURCES}
    ${UI_SOURCES}
    ${AI_SOURCES}
    ${UTILS_SOURCES}
    ${MISSIONS_SOURCES}
    external/imgui/misc/stb_image_impl.cpp
)

# Link libraries (with GUI support)
target_link_libraries(RedNet PRIVATE Threads::Threads ${OPENGL_LIBRARIES} /opt/homebrew/Cellar/glfw/3.4/lib/libglfw.dylib)

# Set output directory
set_target_properties(RedNet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}
    RESOURCE "${APP_ICON}"
)

# Installation
install(TARGETS RedNet
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

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