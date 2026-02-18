cmake_minimum_required(VERSION 3.16)
include(FetchContent)
project(stereo-vision LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Fetch the eigen library for Linear Algebra
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        5.0.0
)
FetchContent_MakeAvailable(eigen)

# JSON for human-friendly config files
# see https://github.com/nlohmann/json?tab=readme-ov-file#examples
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    )
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${OpenCV_LIBS}
        eigen
        nlohmann_json::nlohmann_json
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Determine saving paths, OS dependant
if ((UNIX AND NOT APPLE) OR LINUX)

    # Check for $XDG_CONFIG_HOME, as it might not exist
    if ($ENV{XDG_CONFIG_HOME})
        message("XDG_CONFIG_HOME found")
        set(CONFIG_HOME $ENV{XDG_CONFIG_HOME}/${PROJECT_NAME})
    else()
        # defaults to $HOME/.config
        message("XDG_CONFIG_HOME NOT found, reverting to $HOME/.config")
        set(CONFIG_HOME $ENV{HOME}/.config/${PROJECT_NAME})
    endif()

    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            "PATH_CONFIG_FILES=\"${CONFIG_HOME}\""
            "PATH_DATA_FILES=\"$ENV{HOME}/Documents/${PROJECT_NAME}\""
    )
elseif (WINDOWS)

    # I can't hold my rage against Windows users, let's just crash their systems
    while(TRUE)
        message("I beg you to switch to Linux")
    endwhile()
endif()