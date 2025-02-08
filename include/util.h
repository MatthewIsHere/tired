#ifndef UTIL_H
#define UTIL_H

#include "tired.h"

typedef unsigned int uint;

uint64_t time_now_ms();

uint missing_globals(struct tired_state *state);

#endif /* UTIL_H */