cmake_minimum_required(VERSION 3.0.0)
project(shutdownDLL VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory("shutdownDLL")
add_subdirectory("shutdownWS")
add_subdirectory("uefiscanner")