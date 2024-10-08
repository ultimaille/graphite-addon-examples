cmake_minimum_required(VERSION 3.12...3.26)

project(graphite-addon-examples LANGUAGES C CXX )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

message("OSName: " ${OSName})
message("System name: " ${CMAKE_SYSTEM_NAME})


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
  GIT_TAG        v2.0.0
)
FetchContent_MakeAvailable(ultimaille)

FetchContent_Declare(
  param_parser
  GIT_REPOSITORY https://github.com/ultimaille/param-parser
  GIT_TAG        v1.0.4
)
FetchContent_MakeAvailable(param_parser)

# if (DEFINED UltimaillePath)
#   set(ultimaille_SOURCE_DIR ${UltimaillePath})
#   link_directories("${ultimaille_SOURCE_DIR}/build/lib")
#   message("Get ultimaille at specified path")
# endif()

# message(${ultimaille_SOURCE_DIR})

# Add ultimaille directory to get ultimaille CMakeLists
include_directories(${ultimaille_SOURCE_DIR})
include_directories(${param_parser_SOURCE_DIR})

# Add executable binary
add_executable(delete_facet_addon delete_facet_addon.cpp)
add_executable(smoother_addon smoother_addon.cpp)
add_executable(hex_layer_subdivide_addon hex_layer_subdivide_addon.cpp)
add_executable(hex_scaled_jacobian_addon hex_scaled_jacobian_addon.cpp)
add_executable(step_loader_addon step_loader_addon.cpp)
add_executable(facet_corners_test facet_corners_test.cpp)
add_executable(tag_feature_lines tag_feature_lines_addon.cpp)

# Link binary with ultimaille / param-parser
target_link_libraries(delete_facet_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(smoother_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(hex_layer_subdivide_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(hex_scaled_jacobian_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(step_loader_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(facet_corners_test  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(tag_feature_lines  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)

IF (NOT WIN32)
	target_link_libraries(delete_facet_addon m)
	target_link_libraries(smoother_addon m)
	target_link_libraries(hex_layer_subdivide_addon m)
	target_link_libraries(hex_scaled_jacobian_addon m)
	target_link_libraries(step_loader_addon m)
	target_link_libraries(facet_corners_test m)
	target_link_libraries(tag_feature_lines m)
ENDIF()

if (NOT DEFINED OSName)
  set(OSName ${CMAKE_SYSTEM_NAME})
endif()

set_target_properties(
  delete_facet_addon
    PROPERTIES 
        OUTPUT_NAME "delete_facet_addon_${OSName}"
)

set_target_properties(
  smoother_addon
    PROPERTIES 
        OUTPUT_NAME "smoother_addon_${OSName}"
)

set_target_properties(
  hex_layer_subdivide_addon
    PROPERTIES 
        OUTPUT_NAME "hex_layer_subdivide_addon_${OSName}"
)

set_target_properties(
  hex_scaled_jacobian_addon
    PROPERTIES 
        OUTPUT_NAME "hex_scaled_jacobian_addon_${OSName}"
)

set_target_properties(
  step_loader_addon
    PROPERTIES 
        OUTPUT_NAME "step_loader_addon_${OSName}"
)

set_target_properties(
  facet_corners_test
    PROPERTIES 
        OUTPUT_NAME "facet_corners_test_addon_${OSName}"
)

set_target_properties(
  tag_feature_lines
    PROPERTIES 
        OUTPUT_NAME "tag_feature_lines_addon_${OSName}"
)


file(COPY assets/catorus.geogram DESTINATION .)