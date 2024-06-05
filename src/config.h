
/* Color palettes for the app */
const Color palettes[PALETTE_COUNT][COLOR_COUNT] = {
    [PALETTE_DARK] = {
        [COLOR_BACKGROUND] = 0x000000,
        [COLOR_FOREGROUND] = 0xFFFFFF,
    },
    [PALETTE_LIGHT] = {
        [COLOR_BACKGROUND] = 0xFFFFFF,
        [COLOR_FOREGROUND] = 0x000000,
    },
};

/* The default palette to use */
const EPalette default_palette = PALETTE_LIGHT;

/* The maximum scale for the digit size when rendering */
const float max_digit_scale = 1.0;
