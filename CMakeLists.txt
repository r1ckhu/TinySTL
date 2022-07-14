cmake_minimum_required(VERSION 3.0.0)
project(TinySTL VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-pc-windows-gnu -v")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target x86_64-pc-windows-gnu -v")
add_executable(TinySTL main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
