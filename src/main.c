#include <stdio.h>
#include <stdlib.h>
#include <wayland-util.h>
#include <wayland-client.h>

#include "single-pixel-buffer-v1-protocol.h"
#include "viewporter-protocol.h"
#include "wlr-layer-shell-unstable-v1-protocol.h"

#include "tired.h"
#include "cli.h"
#include "input.h"
#include "output.h"
#include "util.h"
#include "registry.h"

// Pullup
void cleanup();

static struct tired_state state = { 0 };

void screen_dark_callback() {
    int exit_result = EXIT_SUCCESS;
    if (state.command != NULL) {
        exit_result = execute_command(state.command);
    }
    cleanup();
    exit(exit_result);
}
void cancelled_callback() {
    cleanup();
    exit(EXIT_FAILURE);
}
void output_update_callback(struct tired_output* output) {
    if (output->update_count == 1) {
        tired_initialize_overlay(output, &state);
    }
}
void wake_activity_callback() {
    state.cancelling = true;
}



int main(int argc, char** argv) {
    struct cli_options options;
    switch (cli_parse_options(argc, argv, &options)) {
        case CLI_EXIT_FAIL:
            return EXIT_FAILURE;
        case CLI_EXIT_NORMAL:
            return EXIT_SUCCESS;
        default:
            break;
    }

    // Intialize state
    state.hold_time = options.hold_time;
    state.dim_time = options.dim_time;
    state.command = options.command;
    state.brightness = options.brightness;
    state.cancelled_callback = cancelled_callback;
    state.animation_finished_callback = screen_dark_callback;
    wl_list_init(&state.outputs);
    state.display = wl_display_connect(NULL);

    if (!state.display) {
        printf("Failed to connect to Wayland compositor!\n");
        return EXIT_FAILURE;
    }
    
    struct wl_registry *registry = wl_display_get_registry(state.display);
    if (!registry) {
        printf("Failed to get Wayland registry!\n");
        wl_display_disconnect(state.display);
        return EXIT_FAILURE;
    }

    // Add registry listener
    wl_registry_add_listener(registry, &registry_listeners, &state);
    wl_display_roundtrip(state.display); // Ensure registry processing

    if (missing_globals(&state) > 0) {
        wl_registry_destroy(registry);
        cleanup();
        return 1;
    }

    tired_bind_outputs(&state.outputs, output_update_callback);
    tired_bind_inputs(state.seat, wake_activity_callback);
    wl_display_roundtrip(state.display);

    bool running = true;

    while(running) {
        if (wl_display_dispatch(state.display) < 0) {
            printf("an error occured\n");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }
}


void cleanup() {
    struct tired_output *current_output = NULL;
    wl_list_for_each(current_output, &state.outputs, link) {
        tired_destroy_output(current_output);
    };
    wl_list_remove(&state.outputs);
    wl_compositor_destroy(state.compositor);
    zwlr_layer_shell_v1_destroy(state.layer_shell);
    wp_viewporter_destroy(state.viewporter);
    wp_single_pixel_buffer_manager_v1_destroy(state.spix_buf_manager);
    wl_seat_release(state.seat);
    wl_display_disconnect(state.display);
}