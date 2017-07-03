# Project definition
cmake_minimum_required(VERSION 3.8)
project(Shiny-Tests VERSION 0.0.0 LANGUAGES CXX)

# Executable definition and features
add_executable(${PROJECT_NAME} "Main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE Shiny)

# Copy DLLs
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  $<TARGET_FILE:glfw>
                  $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  $<TARGET_FILE:${OPENAL_LIBRARY}>
                  $<TARGET_FILE_DIR:${PROJECT_NAME}>)
