#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <wlr-layer-shell-unstable-v1-protocol.h>
#include <viewporter-protocol.h>
#include <single-pixel-buffer-v1-protocol.h>

#include "registry.h"
#include "tired.h"
#include "output.h"

void global_registry_announce_event(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version) {
    struct tired_state* state = (struct tired_state*) data;
    
    // Bind service objects
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
    } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        state->layer_shell = wl_registry_bind(wl_registry, name, &zwlr_layer_shell_v1_interface, version);
    } else if (strcmp(interface, wp_viewporter_interface.name) == 0) {
        state->viewporter = wl_registry_bind(wl_registry, name, &wp_viewporter_interface, version);
    } else if (strcmp(interface, wp_single_pixel_buffer_manager_v1_interface.name) == 0) {
        state->spix_buf_manager = wl_registry_bind(wl_registry, name, &wp_single_pixel_buffer_manager_v1_interface, version);
    } 
    // Bind IO objects
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        state->seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, version);
    } else if (strcmp(interface, wl_output_interface.name) == 0) {
        struct tired_output* out = calloc(1, sizeof(struct tired_output));
        out->global_name = name;
        out->wl = wl_registry_bind(wl_registry, name, &wl_output_interface, version);
        wl_list_insert(&state->outputs, &out->link);
    }

    
}

void global_registry_removal_event(void *data, struct wl_registry *wl_registry, uint32_t name) {
    printf("Registry Object id: %d has died.\n", name);
}