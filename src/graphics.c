#include <malloc.h>

#include "graphics.h"
#include "util.h"

#include <single-pixel-buffer-v1-protocol.h>
#include <viewporter-protocol.h>
#include <wayland-client.h>

typedef struct {
    struct tired_output *output;
    struct tired_state *state;
} frame_data;

void frame_render_event(void *data, struct wl_callback *callback, uint32_t cb_serial) {
    frame_data *fdata = data;
    repaint_output(fdata->output, fdata->state);
    free(fdata);
}

const struct wl_callback_listener frame_callback_listeners = {
    .done = frame_render_event
};

enum animation_state {
    DIMMING_INITIAL,
    HOLDING,
    DIMMING_SLEEP,
    OFF,
    CANCELLING,
    DONE
};

static enum animation_state animation = DIMMING_INITIAL;
static uint32_t cancellation_alpha = 0;
static uint64_t start_section_ms = 0;

void repaint_output(
    struct tired_output* output, 
    struct tired_state* state
    ) {

    uint64_t now = time_now_ms();
    
    if (start_section_ms == 0) {
        start_section_ms = now;
    }

    uint64_t elapsed = now - start_section_ms;

    uint32_t black = 0xFFFFFFFF;                            // Fully black
    uint32_t target_alpha = (black / 100) * (100-state->brightness);
    uint32_t transparent = 0x0;

    uint32_t alpha = transparent;
    switch (animation) {
        case DIMMING_INITIAL:
                alpha = (uint32_t)(target_alpha / state->dim_time * elapsed);
                if (elapsed >= state->dim_time) {
                    animation = HOLDING;
                    start_section_ms = now;
                }
                break;
        case HOLDING:
                alpha = target_alpha;
                if (elapsed >= state->hold_time) {
                    animation = DIMMING_SLEEP;
                    start_section_ms = now;
                }
                break;
        case DIMMING_SLEEP:
                alpha = (black - target_alpha) / state->dim_time * elapsed + target_alpha;
                if (elapsed >= state->dim_time) {
                    alpha = black;
                    animation = OFF;
                    start_section_ms = now;
                }
                break;
        case CANCELLING:
            alpha = (uint32_t)(cancellation_alpha / CANCELLATION_SPEED * (CANCELLATION_SPEED - elapsed));
            if (elapsed >= CANCELLATION_SPEED) {
                alpha = transparent;
                animation = DONE;
                start_section_ms = now;
            }
            break;
        case OFF:
            alpha = black;
            break;
        case DONE: 
            state->cancelled_callback();
            break;
    }

    if (state->cancelling && cancellation_alpha == 0 && animation != CANCELLING ) {
        // Capture the generated alpha, save to var, and switch animation mode for next render.
        cancellation_alpha = alpha;
        animation = CANCELLING;
        start_section_ms = now;
    }

    // Only draw buffer if we need to make animation changes
    struct wl_buffer *buffer = NULL;
    if (animation != HOLDING) {
        buffer =
            wp_single_pixel_buffer_manager_v1_create_u32_rgba_buffer(
                state->spix_buf_manager,
                0, 0, 0, alpha);

        wl_surface_attach(output->surface, buffer, 0, 0);
        wp_viewport_set_destination(
            output->viewport,
            output->surface_width,
            output->surface_height);
    }
    
    
    if (output->frame_callback) {
        wl_callback_destroy(output->frame_callback);
    }

    if (animation != OFF) {
        frame_data *data = malloc(sizeof(frame_data));
        data->output = output;
        data->state = state;
        output->frame_callback = wl_surface_frame(output->surface);
        wl_callback_add_listener(output->frame_callback, &frame_callback_listeners, data);
    }
    
    wl_surface_damage(output->surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(output->surface);

    if (buffer != NULL) {   
        wl_buffer_destroy(buffer);
    }

    if (animation == OFF) {
        state->animation_finished_callback();
    }
}