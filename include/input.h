#ifndef INPUT_H
#define INPUT_H

#include <wayland-client.h>

void tired_bind_inputs(struct wl_seat *seat,
          void (*activity_callback)(void));

#endif /* INPUT_H */