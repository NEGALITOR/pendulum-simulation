cmake_minimum_required(VERSION 3.20)
project(PendulumSimulation LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies

find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(glfw3  REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

# SOIL2 – fetched and built from source so no pre-built binary is needed
include(FetchContent)
FetchContent_Declare(
    soil2
    GIT_REPOSITORY https://github.com/SpartanJ/SOIL2.git
    GIT_TAG        release-1.20
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(soil2)

# SOIL2 ships no CMakeLists.txt (uses premake), so build it directly from source
add_library(SOIL2 STATIC
    ${soil2_SOURCE_DIR}/src/SOIL2/SOIL2.c
    ${soil2_SOURCE_DIR}/src/SOIL2/image_DXT.c
    ${soil2_SOURCE_DIR}/src/SOIL2/image_helper.c
    ${soil2_SOURCE_DIR}/src/SOIL2/wfETC.c
)
target_include_directories(SOIL2 PUBLIC ${soil2_SOURCE_DIR}/src)
target_link_libraries(SOIL2 PRIVATE OpenGL::GL)
if(APPLE)
    target_link_libraries(SOIL2 PRIVATE "-framework CoreFoundation")
    target_compile_definitions(SOIL2 PRIVATE GL_SILENCE_DEPRECATION)
endif()

# Source Files

file(GLOB SOURCES "src/*.cpp")

add_executable(pendulum ${SOURCES})

# Include paths

target_include_directories(pendulum PRIVATE
    include/
    ${FREETYPE_INCLUDE_DIRS}
)

# Link libraries

target_link_libraries(pendulum PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    assimp::assimp
    Freetype::Freetype
    SOIL2
)

# macOS: silence the deprecated-OpenGL warning from Apple, and cap at 4.1
if(APPLE)
    target_compile_definitions(pendulum PRIVATE
        GL_SILENCE_DEPRECATION
        OPENGL_MAJOR_VERSION=4
        OPENGL_MINOR_VERSION=1
        GLSL_VERSION="410"
    )
else()
    # Linux / Windows support OpenGL 4.3
    target_compile_definitions(pendulum PRIVATE
        OPENGL_MAJOR_VERSION=4
        OPENGL_MINOR_VERSION=3
        GLSL_VERSION="430"
    )
endif()

# Copy runtime assets next to the binary

add_custom_command(TARGET pendulum POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:pendulum>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/fonts"
        "$<TARGET_FILE_DIR:pendulum>/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/models"
        "$<TARGET_FILE_DIR:pendulum>/models"
    COMMENT "Copying runtime assets"
)
