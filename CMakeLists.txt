cmake_minimum_required (VERSION 2.6)
project (RctrlServer)

SET(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

add_executable(RctrlServer ${SRC_DIR}/main.cpp)
target_include_directories(RctrlServer PRIVATE ${SRC_DIR})
target_include_directories(RctrlServer PUBLIC "${PROJECT_SOURCE_DIR}/include")
