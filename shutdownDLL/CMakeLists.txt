cmake_minimum_required(VERSION 3.0.0)
add_definitions(-DUNICODE -D_UNICODE)
FILE(GLOB MyHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB MySources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# Create a target for the library
ADD_LIBRARY(shutdownDLL SHARED ${MySources} ${MyHeaders})
set_target_properties(shutdownDLL PROPERTIES LINKER_LANGUAGE CXX)


