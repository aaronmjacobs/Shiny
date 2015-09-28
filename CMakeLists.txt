### Project Info ###

cmake_minimum_required(VERSION 3.1)
project(Shiny)

# Save state
set(SHINY_BUILD_SHARED_LIB ${BUILD_SHARED_LIBS})
set(SHINY_DEFINITIONS)

# Version info
set(SHINY_VERSION_MAJOR 0)
set(SHINY_VERSION_MINOR 0)
set(SHINY_VERSION_PATCH 0)

# Build type
if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, defaulting to Debug")
   set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

# Windows quirks
if(WIN32)
   add_definitions(-DNOMINMAX)
   list(APPEND SHINY_DEFINITIONS
      -DNOMINMAX
   )
endif(WIN32)

# Information importing / exporting
if(SHINY_BUILD_SHARED_LIB)
   add_definitions(-DSHINY_BUILD_SHARED_LIB)
   list(APPEND SHINY_DEFINITIONS
      -DSHINY_DLL
   )
endif(SHINY_BUILD_SHARED_LIB)

# Notify that Shiny is currently being built
add_definitions(-DSHINY_BUILDING)

# Directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
set(BIN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

# Options
option(SHINY_LOG_INTERNAL "Enable Shiny internal logging" ON)
option(SHINY_LOG_TO_FILE "Enable file logging" OFF)

# Generated content
configure_file (
   "${INCLUDE_DIR}/${PROJECT_NAME}/Defines.h.in"
   "${BIN_INCLUDE_DIR}/${PROJECT_NAME}/Defines.h"
)
configure_file (
   "${INCLUDE_DIR}/${PROJECT_NAME}/Version.h.in"
   "${BIN_INCLUDE_DIR}/${PROJECT_NAME}/Version.h"
)

# Source content
include(sources.cmake)
set(SOURCES)
foreach(SOURCE_NAME ${SOURCE_NAMES})
   list(APPEND SOURCES "${SRC_DIR}/${SOURCE_NAME}")
endforeach(SOURCE_NAME)

include(headers.cmake)
set(HEADERS)
foreach(HEADER_NAME ${HEADER_NAMES})
   list(APPEND HEADERS "${INCLUDE_DIR}/${PROJECT_NAME}/${HEADER_NAME}")
endforeach(HEADER_NAME)
list(APPEND HEADERS "${BIN_INCLUDE_DIR}/${PROJECT_NAME}/Defines.h")
list(APPEND HEADERS "${BIN_INCLUDE_DIR}/${PROJECT_NAME}/Version.h")

set(INCLUDES)
list(APPEND INCLUDES
   ${INCLUDE_DIR}
   ${BIN_INCLUDE_DIR}
)

set(LIBS)
set(COPY_LIBS)

source_group("Dependencies" "dependencies/*")

### Libraries ###

function(attach_lib include source lib)
   if(include)
      list(APPEND INCLUDES ${include})
      set(INCLUDES ${INCLUDES} PARENT_SCOPE)
   endif(include)

   if(source)
      list(APPEND SOURCES ${source})
      set(SOURCES ${SOURCES} PARENT_SCOPE)
   endif(source)

   if(lib)
      list(APPEND LIBS ${lib})
      set(LIBS ${LIBS} PARENT_SCOPE)

      if(BUILD_SHARED_LIBS)
         list(APPEND COPY_LIBS ${lib})
         set(COPY_LIBS ${COPY_LIBS} PARENT_SCOPE)
      endif(BUILD_SHARED_LIBS)
   endif(lib)
endfunction(attach_lib)

function(set_import_locations target lib_release lib_debug imp_release imp_debug)
   if(lib_release)
      set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_RELEASE ${lib_release})
   endif(lib_release)

   if(lib_debug)
      set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_DEBUG ${lib_debug})
   endif(lib_debug)

   if(imp_release)
      set_target_properties(${target} PROPERTIES IMPORTED_IMPLIB_RELEASE ${imp_release})
   endif(imp_release)

   if(imp_debug)
      set_target_properties(${target} PROPERTIES IMPORTED_IMPLIB_DEBUG ${imp_debug})
   endif(imp_debug)
endfunction(set_import_locations)

macro(list_subdirs result curdir)
   file(GLOB children RELATIVE ${curdir} ${curdir}/*)
   set(dirlist "")
   foreach(child ${children})
      if(IS_DIRECTORY ${curdir}/${child})
         list(APPEND dirlist ${child})
      endif(IS_DIRECTORY ${curdir}/${child})
   endforeach(child)
   set(${result} ${dirlist})
endmacro(list_subdirs)

function(copy_headers source dest recurse)
   file(MAKE_DIRECTORY "${dest}")

   file(GLOB HEADERS "${source}/*.h" "${source}/*.hpp" "${source}/*.inl")
   file(COPY ${HEADERS} DESTINATION "${dest}")

   if(recurse)
      list_subdirs(SUBDIRS ${source})
      foreach(subdir ${SUBDIRS})
         copy_headers("${source}/${subdir}" "${dest}/${subdir}" recurse)
      endforeach()
   endif(recurse)
endfunction(copy_headers)

## Integrated ##

# glad
set(GLAD_DIR "${DEP_DIR}/glad")
if(SHINY_BUILD_SHARED_LIB)
   add_definitions(-DGLAD_GLAPI_EXPORT -DGLAD_GLAPI_EXPORT_BUILD)
   list(APPEND SHINY_DEFINITIONS
      -DGLAD_GLAPI_EXPORT
   )
endif(SHINY_BUILD_SHARED_LIB)
attach_lib("${GLAD_DIR}/include" "${GLAD_DIR}/src/glad.c" "${CMAKE_DL_LIBS}")

# GLM
set(GLM_DIR "${DEP_DIR}/glm")
copy_headers(${GLM_DIR}/glm ${BIN_INCLUDE_DIR}/glm true)

# Pempek Assert
set(PEMPEK_DIR "${DEP_DIR}/Assert")
copy_headers(${PEMPEK_DIR}/src ${BIN_INCLUDE_DIR}/pempek false)
attach_lib("${PEMPEK_DIR}/src" "${PEMPEK_DIR}/src/pempek_assert.cpp" "")

# stb
set(STB_DIR "${DEP_DIR}/stb")
copy_headers(${STB_DIR} ${BIN_INCLUDE_DIR}/stb false)

# templog
set(TEMPLOG_DIR "${DEP_DIR}/templog")
if(SHINY_BUILD_SHARED_LIB)
   add_definitions(-DTEMPLOG_BUILD_DLL)
   list(APPEND SHINY_DEFINITIONS
      -DTEMPLOG_DLL
   )
endif(SHINY_BUILD_SHARED_LIB)
copy_headers(${TEMPLOG_DIR} ${BIN_INCLUDE_DIR}/templog false)
attach_lib("${TEMPLOG_DIR}" "${TEMPLOG_DIR}/imp/logging.cpp" "")

# tinyobj
set(TINYOBJ_DIR "${DEP_DIR}/tinyobjloader")
copy_headers(${TINYOBJ_DIR} ${BIN_INCLUDE_DIR}/tinyobj false)
attach_lib("${TINYOBJ_DIR}" "${TINYOBJ_DIR}/tiny_obj_loader.cc" "")

## Static ##

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")

# Boxer
set(BOXER_DIR "${DEP_DIR}/Boxer")
add_subdirectory(${BOXER_DIR})
add_definitions(${BOXER_DEFINITIONS})
list(APPEND SHINY_DEFINITIONS
   ${BOXER_DEFINITIONS}
)
attach_lib("${BOXER_DIR}/include" "" "Boxer")

## Shared ##

set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

# GLFW
set(GLFW_DIR "${DEP_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
set(GLFW_USE_CHDIR OFF CACHE INTERNAL "Make glfwInit chdir to Contents/Resources")
add_subdirectory(${GLFW_DIR})
add_definitions(-DGLFW_INCLUDE_NONE)
list(APPEND SHINY_DEFINITIONS
   -DGLFW_INCLUDE_NONE
)
if(BUILD_SHARED_LIBS)
   add_definitions(-DGLFW_DLL)
   list(APPEND SHINY_DEFINITIONS
      -DGLFW_DLL
   )
endif(BUILD_SHARED_LIBS)
attach_lib("${GLFW_DIR}/include" "" "glfw")

set(BUILD_SHARED_LIBS ${SHINY_BUILD_SHARED_LIB} CACHE INTERNAL "Build shared libraries")

### Building ###

# Includes
include_directories(${INCLUDES})

# Library
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Compiler requirements
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

### Export ###

# Target
set(SHINY_TARGET ${PROJECT_NAME} PARENT_SCOPE)

# Include directories
set(SHINY_INCLUDE_DIRS "${INCLUDES}" PARENT_SCOPE)

# Shared libraries
if(BUILD_SHARED_LIBS)
   list(APPEND COPY_LIBS ${PROJECT_NAME})
endif(BUILD_SHARED_LIBS)
set(SHINY_SHARED_LIBS ${COPY_LIBS} PARENT_SCOPE)

# Definitions
set(SHINY_DEFINITIONS ${SHINY_DEFINITIONS} PARENT_SCOPE)