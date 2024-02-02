cmake_minimum_required(VERSION 3.12...3.26)

project(graphite-addon-examples LANGUAGES C CXX )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("OSName=" ${OSName})

# Get config
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
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

# Add ultimaille directory to get ultimaille CMakeLists
include_directories(${ultimaille_SOURCE_DIR})
include_directories(${param_parser_SOURCE_DIR})

# Add executable binary
add_executable(delete_face_addon delete_face_addon.cpp)

# Link binary with ultimaille / param-parser
target_link_libraries(delete_face_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)

IF (NOT WIN32)
	target_link_libraries(delete_face_addon m)
ENDIF()

set_target_properties(
    delete_face_addon
    PROPERTIES 
        OUTPUT_NAME "delete_facet_addon-${OSName}"
)


file(COPY assets/catorus.geogram DESTINATION .)