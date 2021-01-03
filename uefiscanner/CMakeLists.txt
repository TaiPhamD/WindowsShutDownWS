cmake_minimum_required(VERSION 3.0.0)
add_definitions(-DUNICODE -D_UNICODE)
FILE(GLOB MySources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# Create a target for the library
ADD_EXECUTABLE(uefiscanner ${MySources})


target_link_libraries(uefiscanner user32.lib  advapi32.lib)
set_target_properties(uefiscanner 
                    PROPERTIES 
                    LINKER_LANGUAGE CXX
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")


