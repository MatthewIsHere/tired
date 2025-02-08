#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "tired.h"
#include "output.h"
#include <single-pixel-buffer-v1-protocol.h>

void repaint_output(
    struct tired_output *output, 
    struct tired_state *state);

#endif