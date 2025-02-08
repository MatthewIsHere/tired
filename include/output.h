#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <wayland-client.h>
#include <viewporter-protocol.h>
#include <wlr-layer-shell-unstable-v1-protocol.h>
#include <wayland-util.h>

#include "tired.h"

struct tired_output {
    uint32_t update_count; // Guard variable for the following
    uint32_t global_name;
    int32_t x, y;
    int32_t physical_width, physical_height;
    int32_t subpixel;
    int32_t transform;
    int32_t scale;
    const char *name;
    const char *description;
    const char *make;
    const char *model;

    bool window_setup; // Guard for the following
    struct wl_surface *surface;
    struct zwlr_layer_surface_v1 *layer_surface;
    struct wp_viewport *viewport;
    uint32_t surface_width, surface_height;

    void (*update_callback)(struct tired_output* output);
    struct wl_output *wl;
    struct wl_list link;

    struct wl_callback *frame_callback;
};

void tired_bind_outputs(struct wl_list *output_list,
                        void (*update_callback)(struct tired_output *));


void tired_initialize_overlay(struct tired_output *output, struct tired_state *state);

void tired_destroy_output(struct tired_output *output);

#define LAYER_SURFACE_ANCHOR_CORNERS (    \
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP    | \
    ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT   | \
    ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM | \
    ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT    \
)

#endif /* OUTPUT_H */