#pragma once

#include "common.h"

#include <stdint.h>

typedef struct {
    GFX_GL_BACKEND backend;

    GFX_TEXTURE_FILTER display_filter;
    bool enable_wireframe;
    int32_t line_width;
} GFX_CONFIG;
