cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)

project(sdlTest)

add_executable(${PROJECT_NAME} src/test.cpp)

include(FindPkgConfig)

find_package( Threads )

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
