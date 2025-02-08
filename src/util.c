#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "util.h"

#define CLOCK_MONOTONIC 1 // IDK why it doesnt work with Intelisense...

uint64_t time_now_ms() {
    struct timespec ts = { 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        printf("Could not get system clock, cannot perform animation\n");
        exit(1);
    }
    return (uint64_t)(ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL);
}

uint missing_globals(struct tired_state* state) {
    struct { char* name; bool present;
    } required[] = {
        {"wl_compositor", state->compositor != NULL},
        {"wlr-layer-shell-unstable-v1", state->layer_shell != NULL},
        {"viewporter", state->viewporter != NULL},
        {"single-pixel-buffer-v1", state->spix_buf_manager != NULL},
        {"wl_seat", state->seat != NULL},
    };
    uint missing = 0;
    for (uint i = 0; i < (sizeof(required) / sizeof(required[0]) ); i++) {
        if (!required[i].present) {
            missing++;
            printf("Could not find %s protocol!\n", required[i].name);
        }
    }
    if (missing > 0) {
        printf("Hint: Check compatability with your Wayland compositor.\n");
    }
    return missing;
}