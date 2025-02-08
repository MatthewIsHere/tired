#ifndef REGISTRY_H
#define REGISTRY_H

#include <stdint.h>
#include <wayland-client.h>

void global_registry_announce_event(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version);
void global_registry_removal_event(void *data, struct wl_registry *wl_registry, uint32_t name);

static struct wl_registry_listener registry_listeners = {
    .global = global_registry_announce_event,
    .global_remove = global_registry_removal_event
};

#endif /* REGISTRY_H */