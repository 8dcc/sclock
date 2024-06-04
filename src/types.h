
#ifndef TYPES_H_
#define TYPES_H_ 1

typedef enum {
    PALETTE_DARK  = 0,
    PALETTE_LIGHT = 1,

    /* Last element */
    PALETTE_COUNT,
} EPalette;

typedef enum {
    COLOR_BACKGROUND = 0,
    COLOR_FOREGROUND = 1,

    /* Last element */
    COLOR_COUNT,
} EPaletteColor;

typedef uint32_t Color; /* RGB */

#endif /* TYPES_H_ */
