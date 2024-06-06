
/* Color palettes for the app */
const Color palettes[PALETTE_COUNT][COLOR_COUNT] = {
    [PALETTE_DARK] = {
        [COLOR_BACKGROUND]    = 0x000000,
        [COLOR_GRID]          = 0x333333,
        [COLOR_FOREGROUND]    = 0xFFFFFF,
        [COLOR_PAUSED]        = 0xBBBBBB,
        [COLOR_POMODORO_WORK] = 0xAB3131,
        [COLOR_POMODORO_REST] = 0x507B58,
    },
    [PALETTE_LIGHT] = {
        [COLOR_BACKGROUND]    = 0xFFFFFF,
        [COLOR_GRID]          = 0xAAAAAA,
        [COLOR_FOREGROUND]    = 0x000000,
        [COLOR_PAUSED]        = 0x444444,
        [COLOR_POMODORO_WORK] = 0xAB3131,
        [COLOR_POMODORO_REST] = 0x507B58,
    },
};

/* The default palette to use */
const EPalette default_palette = PALETTE_LIGHT;

/* The space in pixels between dots in the grid */
const unsigned int grid_step = 10;

/* The maximum scale for the digit size when rendering */
const float max_digit_scale = 0.75;
