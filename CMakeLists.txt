cmake_minimum_required(VERSION 3.10)
project(tired VERSION 0.1.0 LANGUAGES C)

# Define source directory
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# Add subdirectory for protocol
add_subdirectory(protocol)

# Define source files
set(SRC_FILES 
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/cli.c
    ${PROJECT_SOURCE_DIR}/input.c
    ${PROJECT_SOURCE_DIR}/output.c
    ${PROJECT_SOURCE_DIR}/graphics.c
    ${PROJECT_SOURCE_DIR}/registry.c
    ${PROJECT_SOURCE_DIR}/util.c
)

# Create executable
add_executable(tired ${SRC_FILES})

# Include directories
target_include_directories(tired PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link required libraries
target_link_libraries(tired PUBLIC 
    wayland-client
    wayland-protocol-generated
)

# Set installation destination
install(TARGETS tired DESTINATION /usr/local/bin)


# Install shell completion scripts system-wide
install(FILES completions/tired.bash DESTINATION /etc/bash_completion.d/)
install(FILES completions/_tired DESTINATION /usr/share/zsh/site-functions/)