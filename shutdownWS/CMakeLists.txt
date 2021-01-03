

FILE(GLOB GO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.go)
file(COPY ${GO_SOURCE}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_SOURCE_DIR}/examples/config_example.json
              ${CMAKE_BINARY_DIR}/dist/config.json)  

# Custom go install 3rd party "Service" lib and build command              
add_custom_command(OUTPUT golang_build.out
        COMMAND ${CMAKE_COMMAND} -E echo "Installing Golang 3rd party lib"
        COMMAND go get github.com/kardianos/service
        COMMAND ${CMAKE_COMMAND} -E echo "Compiling Golang shutdownWS project"
        COMMAND go build -o "${CMAKE_BINARY_DIR}/dist"
)   

              
add_custom_target(shutdownWS ALL
        ${CMAKE_COMMAND} -E echo "Completed GoLang Compile"
        DEPENDS golang_build.out)


add_dependencies(shutdownWS shutdownDLL)

