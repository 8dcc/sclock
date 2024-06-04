
/* sclock user configuration */

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

const EPalette default_palette = PALETTE_LIGHT;
