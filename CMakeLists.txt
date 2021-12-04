cmake_minimum_required(VERSION 3.12)

project(alemungula VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug
    CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

add_subdirectory("lib/SFML")

include_directories(
  "lib/SFML/include"
)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} 
  sfml-graphics
  sfml-system
  sfml-window
  sfml-graphics
  sfml-network
  sfml-audio
)
