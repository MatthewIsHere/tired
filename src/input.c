#include "input.h"

void keyboard_pressed_event(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    void (*activity_callback)(void) = data;

    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        wl_keyboard_release(keyboard);
        activity_callback();
    }
}

void keyboard_modifiers_event() {};
void keyboard_enter_event() {};
void keyboard_leave_event() {};
void keyboard_repeat_event() {};
void keyboard_keymap_event() {};

const struct wl_keyboard_listener keyboard_event_listeners = {
    .key = keyboard_pressed_event,
    .modifiers = keyboard_modifiers_event,
    .enter = keyboard_enter_event,
    .leave = keyboard_leave_event,
    .repeat_info = keyboard_repeat_event,
    .keymap = keyboard_keymap_event
};


void pointer_button_event(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    void (*activity_callback)(void) = data;
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        wl_pointer_release(pointer);
        activity_callback();
    }
}
void pointer_motion_event(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    void (*activity_callback)(void) = data;
    wl_pointer_release(pointer);
    activity_callback();
}

void pointer_enter_event() {}
void pointer_leave_event() {}
void pointer_axis_event() {}
void pointer_axis_discrete_event() {}
void pointer_axis_relative_direction_event() {}
void pointer_axis_source_event() {}
void pointer_axis_120_event() {}
void pointer_frame_event() {}

const struct wl_pointer_listener pointer_event_listeners = {
    .button = pointer_button_event,
    .motion = pointer_motion_event,
    .enter = pointer_enter_event,
    .leave = pointer_leave_event,
    .axis = pointer_axis_event,
    .axis_discrete = pointer_axis_discrete_event,
    .axis_relative_direction = pointer_axis_relative_direction_event,
    .axis_source = pointer_axis_source_event,
    .axis_value120 = pointer_axis_120_event,
    .frame = pointer_frame_event
};

void tired_bind_inputs(struct wl_seat *seat, void (*activity_callback)(void)) {
    struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
    struct wl_pointer *pointer = wl_seat_get_pointer(seat);

    wl_keyboard_add_listener(keyboard, &keyboard_event_listeners, activity_callback);
    wl_pointer_add_listener(pointer, &pointer_event_listeners, activity_callback);
}