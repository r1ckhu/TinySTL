cmake_minimum_required(VERSION 3.0.0)
project(TinySTL VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-pc-windows-gnu -v")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target x86_64-pc-windows-gnu -v")
aux_source_directory(src SRC_SUB)
aux_source_directory(. SRC_CUR)
add_executable(TinySTL ${SRC_SUB} ${SRC_CUR})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
   includes
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CPack)
