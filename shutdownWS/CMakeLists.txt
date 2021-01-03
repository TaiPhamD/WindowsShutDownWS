
project(shutdownDLL VERSION 0.1.0)

set(TARGET shutdownWS)
set(GOPATH ${CMAKE_CURRENT_BINARY_DIR})
set(SRCS main.go)

# Custom go install 3rd party "Service" lib and build command              
add_custom_command(OUTPUT golang_build.out
        DEPENDS ${SRCS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "Installing Golang 3rd party lib"
        COMMAND go get github.com/kardianos/service
        COMMAND ${CMAKE_COMMAND} -E echo "Compiling Golang shutdownWS project"
        COMMAND go build -o "${CMAKE_BINARY_DIR}/dist"
)   


add_custom_target(shutdownWS ALL
        ${CMAKE_COMMAND} -E echo "Completed GoLang Compile"
        DEPENDS golang_build.out)


