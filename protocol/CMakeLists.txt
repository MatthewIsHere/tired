# Gotta be honest bro, I hate CMake. I ChatGPT'd this crap, originally from emersion/chayang. This is not my work, sorry.

# 1) Use pkg-config to find wayland-scanner and wayland-protocols
find_package(PkgConfig REQUIRED)

pkg_check_modules(WAYLAND_SCANNER REQUIRED wayland-scanner>=1.14.91)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.14)

# 2) Retrieve the pkgdatadir from wayland-protocols
execute_process(
    COMMAND "${PKG_CONFIG_EXECUTABLE}" --variable=pkgdatadir wayland-protocols
    OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DATADIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 3) Define the list of XML protocol files
set(client_protocols
    "${WAYLAND_PROTOCOLS_DATADIR}/stable/viewporter/viewporter.xml"
    "${WAYLAND_PROTOCOLS_DATADIR}/stable/xdg-shell/xdg-shell.xml"
    "${WAYLAND_PROTOCOLS_DATADIR}/staging/single-pixel-buffer/single-pixel-buffer-v1.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1.xml"
)

# 3.1) Locate the wayland-scanner executable
find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner
    HINTS ${WAYLAND_SCANNER_BINDIR} ${WAYLAND_SCANNER_LIBEXECDIR}
)
if (NOT WAYLAND_SCANNER_EXECUTABLE)
    message(FATAL_ERROR "wayland-scanner not found!")
endif()

set(protocol_generated_srcs "")
set(protocol_generated_hdrs "")

# 3.2) For each XML, create commands to generate the .c and .h
foreach(xml_file IN LISTS client_protocols)
    get_filename_component(xml_name_we "${xml_file}" NAME_WE)
    
    set(gen_c "${CMAKE_CURRENT_BINARY_DIR}/${xml_name_we}-protocol.c")
    set(gen_h "${CMAKE_CURRENT_BINARY_DIR}/${xml_name_we}-protocol.h")

    # Generate .c from the protocol XML
    add_custom_command(
        OUTPUT  "${gen_c}"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${xml_file}" "${gen_c}"
        DEPENDS "${xml_file}"
        COMMENT "Generating C code from ${xml_file}"
        VERBATIM
    )

    # Generate .h from the protocol XML
    add_custom_command(
        OUTPUT  "${gen_h}"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${xml_file}" "${gen_h}"
        DEPENDS "${xml_file}"
        COMMENT "Generating header from ${xml_file}"
        VERBATIM
    )

    list(APPEND protocol_generated_srcs "${gen_c}")
    list(APPEND protocol_generated_hdrs "${gen_h}")
endforeach()

# 4) Create a static library (or another type) from the generated sources
add_library(wayland-protocol-generated STATIC
    ${protocol_generated_srcs}
    ${protocol_generated_hdrs}
)

# 5) Make sure the generated headers can be found by other targets
target_include_directories(wayland-protocol-generated PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Optionally print out the discovered protocol datadir for debugging
message(STATUS "WAYLAND_PROTOCOLS_DATADIR = ${WAYLAND_PROTOCOLS_DATADIR}")
