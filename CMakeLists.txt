### Project Info ###

cmake_minimum_required(VERSION 3.1)
project(Shiny)
set(PROJECT_DISPLAY_NAME ${PROJECT_NAME})

# Version info
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_MICRO 0)
set(VERSION_BUILD 1)

# Build type
if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, defaulting to Debug")
   set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

# Compiler requirements
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(LINUX true)
else(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(LINUX false)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(64_BIT true)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(64_BIT false)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

# rpath handling
if(LINUX)
   set(CMAKE_INSTALL_RPATH "$ORIGIN/")
endif(LINUX)

# Windows quirks
if(WIN32)
   add_definitions(-DNOMINMAX)
endif(WIN32)

# Directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")
set(BIN_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")
set(DATA_DIR_NAME "data")
set(DATA_DIR "${PROJECT_SOURCE_DIR}/${DATA_DIR_NAME}")
set(RES_DIR "${PROJECT_SOURCE_DIR}/resources")
set(BIN_RES_DIR "${PROJECT_BINARY_DIR}/resources")

# Options
option(SHINY_LOG_TO_FILE "Enable file logging" OFF)

# Generated content
configure_file (
   "${SRC_DIR}/Constants.h.in"
   "${BIN_INCLUDE_DIR}/Constants.h"
)

# Source content
include(sources.cmake)
foreach(SOURCE_NAME ${SOURCE_NAMES})
   list(APPEND SOURCES "${SRC_DIR}/${SOURCE_NAME}")
endforeach(SOURCE_NAME)

include(headers.cmake)
foreach(HEADER_NAME ${HEADER_NAMES})
   list(APPEND HEADERS "${SRC_DIR}/${HEADER_NAME}")
endforeach(HEADER_NAME)

list(APPEND INCLUDES
   ${SRC_DIR}
   ${BIN_INCLUDE_DIR}
)

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

function(copy_headers source dest)
   file(MAKE_DIRECTORY "${dest}")

   file(GLOB HEADERS "${source}/*.h" "${source}/*.hpp" "${source}/*.inl")
   file(COPY ${HEADERS} DESTINATION "${dest}")

   list_subdirs(SUBDIRS ${source})
   foreach(subdir ${SUBDIRS})
      copy_headers("${source}/${subdir}" "${dest}/${subdir}")
   endforeach()
endfunction(copy_headers)

## Integrated ##

# glad
set(GLAD_DIR "${DEP_DIR}/glad")
attach_lib("${GLAD_DIR}/include" "${GLAD_DIR}/src/glad.c" "${CMAKE_DL_LIBS}")

# GLM
set(GLM_DIR "${DEP_DIR}/glm")
copy_headers(${GLM_DIR}/glm ${BIN_INCLUDE_DIR}/glm)

# Pempek Assert
set(PEMPEK_DIR "${DEP_DIR}/PempekAssert")
attach_lib("${PEMPEK_DIR}/include" "${PEMPEK_DIR}/lib/pempek_assert.cpp" "")

# templog
set(TEMPLOG_DIR "${DEP_DIR}/templog")
attach_lib("${TEMPLOG_DIR}/include" "${TEMPLOG_DIR}/imp/logging.cpp" "")

# tinyobj
set(TINYOBJ_DIR "${DEP_DIR}/tinyobj")
attach_lib("${TINYOBJ_DIR}/include" "${TINYOBJ_DIR}/src/tiny_obj_loader.cc" "")

## Static ##

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")

# Boxer
set(BOXER_DIR "${DEP_DIR}/Boxer")
add_subdirectory(${BOXER_DIR})
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
attach_lib("${GLFW_DIR}/include" "" "glfw")

### Building ###

# Includes
include_directories(${INCLUDES})

# Binary
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBS})

### Post-Build ###

# Symlink data folder
if(EXISTS ${DATA_DIR})
   if(WIN32)
      set(SYMLINK_COMMAND "${RES_DIR}/win/symlink.bat")
   else(WIN32)
      set(SYMLINK_COMMAND ${CMAKE_COMMAND} -E create_symlink)
   endif(WIN32)
   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${SYMLINK_COMMAND} "${DATA_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR_NAME}"
   )
endif(EXISTS ${DATA_DIR})

# Copy DLLs
if(WIN32)
   foreach(COPY_LIB ${COPY_LIBS})
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${COPY_LIB}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
      )
   endforeach(COPY_LIB)
endif(WIN32)

### Install ###

if(APPLE)
   set(APP_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
   set(CONTENTS_DIR "${APP_DIR}/Contents")
   set(MACOS_DIR "${CONTENTS_DIR}/MacOS")
   set(RESOURCES_DIR "${CONTENTS_DIR}/Resources")

   # Generate the .app file
   install(CODE "
      file(MAKE_DIRECTORY \"${MACOS_DIR}\")
      file(MAKE_DIRECTORY \"${RESOURCES_DIR}\")")

   # Generate the Info.plist file (at build time, not install time)
   configure_file(
     "${RES_DIR}/osx/Info.plist.in"
     "${BIN_RES_DIR}/Info.plist"
   )

   # Copy the generated Info.plist file
   install(FILES "${BIN_RES_DIR}/Info.plist" DESTINATION "${CONTENTS_DIR}")

   # Copy the icon
   set(ICON_FILE "${RES_DIR}/osx/${PROJECT_NAME}.icns")
   if(EXISTS ${ICON_FILE})
      install(FILES "${ICON_FILE}" DESTINATION "${RESOURCES_DIR}")
   endif(EXISTS ${ICON_FILE})

   # Copy the data folder
   if(EXISTS ${DATA_DIR})
      install(DIRECTORY "${DATA_DIR}" DESTINATION "${MACOS_DIR}")
   endif(EXISTS ${DATA_DIR})

   # Copy the executable (not install, so that the internal lib paths don't change)
   install(FILES "$<TARGET_FILE:${PROJECT_NAME}>" DESTINATION "${MACOS_DIR}"
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

   # Fix the bundle (copies and fixes shared libraries)
   install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"${APP_DIR}\" \"\" \"\")")
endif(APPLE)

if(WIN32)
   # Copy the data folder
   if(EXISTS ${DATA_DIR})
      install(DIRECTORY "${DATA_DIR}" DESTINATION ".")
   endif(EXISTS ${DATA_DIR})

   # Install the executable
   install(TARGETS ${PROJECT_NAME} DESTINATION ".")

   # Install all shared libraries
   foreach(COPY_LIB ${COPY_LIBS})
      install(FILES "$<TARGET_FILE:${COPY_LIB}>" DESTINATION ".")
   endforeach(COPY_LIB)

   # Install system libraries
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true) # Let us choose the install location
   include(InstallRequiredSystemLibraries)
   if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ".")
   endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
endif(WIN32)

if(LINUX)
   # Copy the data folder
   if(EXISTS ${DATA_DIR})
      install(DIRECTORY "${DATA_DIR}" DESTINATION ".")
   endif(EXISTS ${DATA_DIR})

   # Copy the executable (not install, so that the rpath doesn't change)
   install(FILES "$<TARGET_FILE:${PROJECT_NAME}>" DESTINATION "${CMAKE_INSTALL_PREFIX}"
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

   # Fix the bundle (in order to copy shared libraries)
   # Note: verify_app (called at the end of fixup_bundle) WILL fail, due to referencing external libraries
   # In order to ignore the error / continue, replace the verify_app function with one that does nothing
   install(CODE "
      include(BundleUtilities)
      function(verify_app app)
         message(STATUS \"(Not actually verifying...)\")
      endfunction(verify_app)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\" \"\" \"\")")

   # Install the executable (this time, fixing the rpath)
   install(TARGETS ${PROJECT_NAME} DESTINATION ".")

   # Fix the bundle (just to do a proper verify)
   install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\" \"\" \"\")")
endif(LINUX)
