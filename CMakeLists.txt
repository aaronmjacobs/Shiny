cmake_minimum_required(VERSION 3.8)

### Project Info ###

# Project definition
project(Shiny VERSION 0.0.0 LANGUAGES CXX C)

# Requirements
if(${BUILD_SHARED_LIBS})
   message(SEND_ERROR "Shiny can not be used as a shared library due to its extensive use of the STL in its public API")
endif(${BUILD_SHARED_LIBS})

if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, defaulting to Debug")
   set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

# Directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries")
set(BIN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Include")

# Options
option(SHINY_LOG_INTERNAL "Enable Shiny internal logging" ON)
option(SHINY_LOG_MSVC_STYLE "Format logs for MSVC" ${MSVC})

### Source Content ###

# Generated headers
set(GENERATED_HEADERS)
function(configure_header name)
   set(GENERATED_HEADER "${BIN_INCLUDE_DIR}/${PROJECT_NAME}/${name}.h")
   configure_file(
      "${INCLUDE_DIR}/${PROJECT_NAME}/${name}.h.in"
      ${GENERATED_HEADER}
   )
   list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
   set(GENERATED_HEADERS ${GENERATED_HEADERS} PARENT_SCOPE)
endfunction(configure_header)

configure_header("Defines")
configure_header("Version")

# Headers
include(Headers.cmake)
set(HEADERS)
foreach(HEADER_NAME ${HEADER_NAMES})
   list(APPEND HEADERS "${INCLUDE_DIR}/${PROJECT_NAME}/${HEADER_NAME}")
endforeach(HEADER_NAME)

# Sources
include(Sources.cmake)
set(SOURCES)
foreach(SOURCE_NAME ${SOURCE_NAMES})
   list(APPEND SOURCES "${SRC_DIR}/${SOURCE_NAME}")
endforeach(SOURCE_NAME)

# Source groups
source_group(TREE "${BIN_INCLUDE_DIR}/${PROJECT_NAME}" PREFIX "Generated Headers" FILES ${GENERATED_HEADERS})
source_group(TREE "${INCLUDE_DIR}/${PROJECT_NAME}" PREFIX "Headers" FILES ${HEADERS})
source_group(TREE "${SRC_DIR}" PREFIX "Sources" FILES ${SOURCES})

### Target Info ###

# Shiny library
add_library(${PROJECT_NAME} ${GENERATED_HEADERS} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${BIN_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_NAME})

# Windows quirks
if(WIN32)
   target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif(WIN32)

# Notify that Shiny is currently being built
target_compile_definitions(${PROJECT_NAME} PRIVATE SHINY_BUILDING)

# C++ version
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

### Libraries ###

# Save state
set(SHINY_BUILD_SHARED_LIB ${BUILD_SHARED_LIBS})

# Source group
source_group("Libraries" "${LIB_DIR}/*")

## Integrated / Header Only ##

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
target_sources(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/src/glad.c")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})

# GLM
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory(${GLM_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:glm,INTERFACE_INCLUDE_DIRECTORIES>)

# PPK_ASSERT
set(PPK_DIR "${LIB_DIR}/PPK_ASSERT")
target_sources(${PROJECT_NAME} PRIVATE "${PPK_DIR}/src/ppk_assert.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "${PPK_DIR}/src")

# stb
set(STB_DIR "${LIB_DIR}/stb")
target_include_directories(${PROJECT_NAME} PUBLIC "${STB_DIR}")

# templog
set(TEMPLOG_DIR "${LIB_DIR}/templog")
target_sources(${PROJECT_NAME} PRIVATE "${TEMPLOG_DIR}/imp/logging.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "${TEMPLOG_DIR}")

## Static ##

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")

# Boxer
set(BOXER_DIR "${LIB_DIR}/Boxer")
add_subdirectory(${BOXER_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:Boxer,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(${PROJECT_NAME} PUBLIC Boxer $<TARGET_PROPERTY:Boxer,INTERFACE_LINK_LIBRARIES>)
target_compile_features(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:Boxer,INTERFACE_COMPILE_FEATURES>)
target_compile_definitions(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:Boxer,INTERFACE_COMPILE_DEFINITIONS>)

# tinyobj
set(TINYOBJ_DIR "${LIB_DIR}/tinyobjloader")
add_subdirectory(${TINYOBJ_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:tinyobjloader,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(${PROJECT_NAME} PUBLIC tinyobjloader)

## Shared ##

set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
set(GLFW_USE_CHDIR OFF CACHE INTERNAL "Make glfwInit chdir to Contents/Resources")
add_subdirectory(${GLFW_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_NONE)
if(BUILD_SHARED_LIBS)
   target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_DLL)
endif(BUILD_SHARED_LIBS)
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# OpenAL
set(OPENAL_DIR "${LIB_DIR}/openal-soft")
set(ALSOFT_UTILS OFF CACHE INTERNAL "Build and install utility programs")
set(ALSOFT_NO_CONFIG_UTIL ON CACHE INTERNAL "Disable building the alsoft-config utility")
set(ALSOFT_EXAMPLES OFF CACHE INTERNAL "Build and install example programs")
set(ALSOFT_CONFIG OFF CACHE INTERNAL "Install alsoft.conf sample configuration file")
set(ALSOFT_HRTF_DEFS OFF CACHE INTERNAL "Install HRTF definition files")
set(ALSOFT_INSTALL OFF CACHE INTERNAL "Install headers and libraries")
add_subdirectory(${OPENAL_DIR})
if(WIN32)
   set(OPENAL_LIBRARY OpenAL32)
else(WIN32)
   set(OPENAL_LIBRARY openal)
endif(WIN32)
if(NOT BUILD_SHARED_LIBS)
   target_compile_definitions(${PROJECT_NAME} PUBLIC AL_LIBTYPE_STATIC)
endif(NOT BUILD_SHARED_LIBS)
target_include_directories(${PROJECT_NAME} PUBLIC "${OPENAL_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENAL_LIBRARY})

set(BUILD_SHARED_LIBS ${SHINY_BUILD_SHARED_LIB} CACHE INTERNAL "Build shared libraries")
