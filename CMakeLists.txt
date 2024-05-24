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
  # GIT_TAG        v1.1.0
  GIT_TAG        geom
)
FetchContent_MakeAvailable(ultimaille)

FetchContent_Declare(
  param_parser
  GIT_REPOSITORY https://github.com/ultimaille/param-parser
  GIT_TAG        v1.0.1
)
FetchContent_MakeAvailable(param_parser)



# Add ultimaille directory to get ultimaille CMakeLists
include_directories(${ultimaille_SOURCE_DIR})
include_directories(${param_parser_SOURCE_DIR})

# Add executable binary
add_executable(delete_facet_addon delete_facet_addon.cpp)
add_executable(smoother_addon smoother_addon.cpp)
add_executable(hex_layer_subdivide hex_layer_subdivide.cpp)
add_executable(hex_scaled_jacobian hex_scaled_jacobian.cpp)
add_executable(step_loader step_loader.cpp)

# Link binary with ultimaille / param-parser
target_link_libraries(delete_facet_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(smoother_addon  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(hex_layer_subdivide  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(hex_scaled_jacobian  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(step_loader  ${CMAKE_DL_LIBS} param-parser ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)

IF (NOT WIN32)
	target_link_libraries(delete_facet_addon m)
	target_link_libraries(smoother_addon m)
	target_link_libraries(hex_layer_subdivide m)
	target_link_libraries(hex_scaled_jacobian m)
	target_link_libraries(step_loader m)
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
  hex_layer_subdivide
    PROPERTIES 
        OUTPUT_NAME "hex_layer_subdivide_${OSName}"
)

set_target_properties(
  hex_scaled_jacobian
    PROPERTIES 
        OUTPUT_NAME "hex_scaled_jacobian_${OSName}"
)


file(COPY assets/catorus.geogram DESTINATION .)