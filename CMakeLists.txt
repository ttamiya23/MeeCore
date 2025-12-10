cmake_minimum_required(VERSION 3.20)

# 1. Define the Project
project(MeeCore C)

# 2. Define the Library
# STATIC means it compiles to a .a file.
# We do not include 'ports/' here by default; the user adds their specific port.
file(GLOB MEECORE_SOURCES CONFIGURE_DEPENDS 
    "src/core/*.c"
)
add_library(MeeCore STATIC ${MEECORE_SOURCES})

# 3. Setup Include Directories
# PUBLIC = Any project using MeeCore automatically gets "include/" added to their path.
target_include_directories(MeeCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. The Configuration Injection Mechanism
# If the user defines MEECORE_CONFIG_PATH in their main CMakeLists.txt,
# we add it to the include path and trigger the macro.
if(DEFINED MEECORE_CONFIG_PATH)
    message(STATUS "MeeCore: Using custom config at ${MEECORE_CONFIG_PATH}")
    
    # Add the user's config folder to the search path
    target_include_directories(MeeCore PRIVATE ${MEECORE_CONFIG_PATH})
    
    # Define the macro that tells meecore_config.h to look for the user file
    target_compile_definitions(MeeCore PRIVATE MEECORE_USE_PROJECT_CONFIG)
endif()