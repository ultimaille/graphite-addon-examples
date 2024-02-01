cmake_minimum_required(VERSION 3.12...3.26)

project(graphite-addon-examples LANGUAGES C CXX )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get config
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

# Try to find OpenMP package (if installed)
find_package(OpenMP COMPONENTS CXX)

# Display warning and error levels
if (MSVC)
    # warning level 4 (and all warnings as errors, /WX)
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Get development version of ultimaille directly from git repository 
include(FetchContent)
FetchContent_Declare(
  ultimaille
  GIT_REPOSITORY https://github.com/ssloy/ultimaille
  GIT_TAG        v1.1.0
)
FetchContent_MakeAvailable(ultimaille)

FetchContent_Declare(
  param_parser
  GIT_REPOSITORY https://github.com/ultimaille/param-parser
  GIT_TAG        master
)
FetchContent_MakeAvailable(param_parser)

message(${ultimaille_SOURCE_DIR})
message(${param_parser_SOURCE_DIR})
# Add ultimaille directory to get ultimaille CMakeLists
include_directories(${ultimaille_SOURCE_DIR})
include_directories(${param_parser_SOURCE_DIR})

# Add executable binary
add_executable(delete_face_addon delete_face_addon.cpp)

# Link binary with ultimaille
target_link_libraries(delete_face_addon  ${CMAKE_DL_LIBS} param-parser ultimaille m $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)