
#ifndef TYPES_H_
#define TYPES_H_ 1

typedef enum {
    MODE_CLOCK = 0,
    MODE_STOPWATCH,
} EMode;

typedef enum {
    PALETTE_DARK = 0,
    PALETTE_LIGHT,

    /* Last element */
    PALETTE_COUNT,
} EPalette;

typedef enum {
    COLOR_BACKGROUND = 0,
    COLOR_GRID,
    COLOR_FOREGROUND,
    COLOR_PAUSED,

    /* Last element */
    COLOR_COUNT,
} EPaletteColor;

typedef uint32_t Color; /* RGB */

#endif /* TYPES_H_ */
