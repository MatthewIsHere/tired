#include <malloc.h>
#include <wayland-client.h>
#include <viewporter-protocol.h>
#include <wlr-layer-shell-unstable-v1-protocol.h>
#include "output.h"
#include "graphics.h"

typedef struct {
    struct tired_output *output;
    struct tired_state *state;
} callback_data;

void output_name_event(void *data, struct wl_output *output, const char *name) {
    struct tired_output *out = data;
    out->name = name;
}
void output_description_event(void *data, struct wl_output *output, const char *description) {
    struct tired_output *out = data;
    out->description = description;
}
void output_geometry_event(void *data,
                           struct wl_output *output,
                           int32_t x,
                           int32_t y,
                           int32_t phys_width,
                           int32_t phys_height,
                           int32_t subpixel,
                           const char *make,
                           const char *model,
                           int32_t transform) {
    struct tired_output *out = data;
    out->x = x; out->y = y;
    out->physical_width = phys_width; out->physical_height = phys_height;
    out->subpixel = subpixel;
    out->make = make; out->model = model;
    out->transform = transform;
}
void output_mode_event(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    // Noop
}
void output_scale_event(void *data, struct wl_output *output, int32_t scale_factor) {
    struct tired_output *out = data;
    out->scale = scale_factor;
}
void output_properties_update(void *data, struct wl_output *output) {
    struct tired_output *out = data;
    out->update_count++;
    out->update_callback(out);
}


struct wl_output_listener output_listeners = {
    .name = output_name_event,
    .description = output_description_event,
    .geometry = output_geometry_event,
    .mode = output_mode_event,
    .scale = output_scale_event,
    .done = output_properties_update
};

void layer_surface_configure_event(
    void *data, 
    struct zwlr_layer_surface_v1 *surface, 
    uint32_t serial, 
    uint32_t width, 
    uint32_t height) {
    callback_data* cb = data;
    
    cb->output->surface_width = width;
    cb->output->surface_height = height;

    zwlr_layer_surface_v1_ack_configure(surface, serial);
    repaint_output(cb->output, cb->state);
}

void layer_surface_closed_event(
    void *data,
    struct zwlr_layer_surface_v1 *surface) {
    callback_data *cb = data;
    zwlr_layer_surface_v1_destroy(cb->output->layer_surface);
    free(cb);
}

struct zwlr_layer_surface_v1_listener layer_surface_listeners = {
    .configure = layer_surface_configure_event,
    .closed = layer_surface_closed_event
};

void tired_bind_outputs(struct wl_list *output_list,
                        void (*update_callback)(struct tired_output *)) {
    struct tired_output* out;
    wl_list_for_each_reverse(out, output_list, link) {
        out->update_callback=update_callback;
        wl_output_add_listener(out->wl, &output_listeners, out);
    }
}



void tired_initialize_overlay(struct tired_output *output, struct tired_state *state) {
    output->surface = wl_compositor_create_surface(state->compositor);

    output->layer_surface = zwlr_layer_shell_v1_get_layer_surface(
        state->layer_shell,
        output->surface,
        output->wl,
        ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY,
        "tired");

    callback_data* data = malloc(sizeof(callback_data));
    data->output = output;
    data->state = state;
    
    zwlr_layer_surface_v1_add_listener(
        output->layer_surface,
        &layer_surface_listeners,
        data
    );

    output->viewport = wp_viewporter_get_viewport(state->viewporter, output->surface);
    zwlr_layer_surface_v1_set_anchor(output->layer_surface, LAYER_SURFACE_ANCHOR_CORNERS);
    zwlr_layer_surface_v1_set_keyboard_interactivity(output->layer_surface, ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_EXCLUSIVE);
    zwlr_layer_surface_v1_set_exclusive_zone(output->layer_surface, 10);

    wl_surface_commit(output->surface);
    output->window_setup = true;
}

void tired_destroy_output(struct tired_output* output) {
    if (output == NULL) return;
    if (output->viewport != NULL) {
        wp_viewport_destroy(output->viewport);
    }
    if (output->layer_surface != NULL) {
        zwlr_layer_surface_v1_destroy(output->layer_surface);
    }
    if (output->surface != NULL) {
        wl_surface_destroy(output->surface);
    }
    if (output->wl != NULL) {
        wl_output_destroy(output->wl);
    }
    free(output);
}