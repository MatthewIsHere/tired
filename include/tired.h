#ifndef TIRED_H
#define TIRED_H

#include <stdint.h>
#include <stdbool.h>
#include <wayland-util.h>

#define DEFAULT_HOLD 15000
#define DEFAULT_SPEED 250
#define CANCELLATION_SPEED 200
#define DEFAULT_BRIGHTNESS 30

struct tired_state {
    struct wl_display *display;
    struct wl_compositor *compositor;
    struct zwlr_layer_shell_v1 *layer_shell;
    struct wp_viewporter *viewporter;
    struct wp_single_pixel_buffer_manager_v1 *spix_buf_manager;
    struct wl_seat* seat;
    struct wl_list outputs;
    
    bool cancelling;
    void (*cancelled_callback)(void);
    void (*animation_finished_callback)(void);


    // CLI options
    uint32_t hold_time;    // User-defined hold time
    uint32_t dim_time;     // User-defined dim speed
    uint32_t brightness;   // User-defined brightness
    char *command;         // Command to execute before exit
};

#endif /* TIRED_H */