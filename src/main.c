
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>

#define FPS 60

#include "types.h"
#include "config.h"
#include "digits.h"

/*----------------------------------------------------------------------------*/
/* Globals */

static SDL_Window* sdl_window = NULL;
static EPalette palette       = default_palette;

/*----------------------------------------------------------------------------*/
/* Misc helper functions */

static void die(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    vfprintf(stderr, fmt, va);
    putc('\n', stderr);

    if (sdl_window != NULL)
        SDL_DestroyWindow(sdl_window);

    SDL_Quit();
    exit(1);
}

/*----------------------------------------------------------------------------*/
/* SDL helper functions */

static inline void set_render_color(SDL_Renderer* rend, uint32_t col) {
    const uint8_t r = (col >> 16) & 0xFF;
    const uint8_t g = (col >> 8) & 0xFF;
    const uint8_t b = (col >> 0) & 0xFF;
    const uint8_t a = 255;
    SDL_SetRenderDrawColor(rend, r, g, b, a);
}

static inline void set_texture_color(SDL_Texture* texture, uint32_t col) {
    const uint8_t r = (col >> 16) & 0xFF;
    const uint8_t g = (col >> 8) & 0xFF;
    const uint8_t b = (col >> 0) & 0xFF;
    SDL_SetTextureColorMod(texture, r, g, b);
}

static void draw_digit(SDL_Renderer* rend, SDL_Texture* texture, int x, int y,
                       float scale, int digit_idx) {
    const SDL_Rect src_rect = {
        digit_idx * DIGIT_WIDTH,
        0,
        DIGIT_WIDTH,
        DIGIT_HEIGHT,
    };
    const SDL_Rect dst_rect = {
        x,
        y,
        scale * (float)DIGIT_WIDTH,
        scale * (float)DIGIT_HEIGHT,
    };

    SDL_RenderCopy(rend, texture, &src_rect, &dst_rect);
}

/* Return the scale for the text size, depending on it's width, and the window
 * size. */
static float get_text_scale(int win_w, int win_h, int txt_w) {
    float aspect_ratio_txt = (float)txt_w / (float)DIGIT_HEIGHT;
    float aspect_ratio_win = (float)win_w / (float)win_h;

    float scale = (aspect_ratio_txt > aspect_ratio_win)
                    ? (float)win_w / (float)txt_w
                    : (float)win_h / (float)DIGIT_HEIGHT;

    return (scale < MAX_DIGIT_SCALE) ? scale : MAX_DIGIT_SCALE;
}

/* Return the index for the `digits' texture depending on the character to be
 * drawn. The index can be passed to the `draw_digit' function. */
static int char_to_idx(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c == ':')
        return 10;

    return -1;
}

static void draw_string(SDL_Renderer* rend, SDL_Texture* texture,
                        const char* str) {
    int win_w, win_h;
    SDL_GetWindowSize(sdl_window, &win_w, &win_h);

    const int txt_len     = strlen(str);
    const int txt_w       = txt_len * DIGIT_WIDTH;
    const float txt_scale = get_text_scale(win_w, win_h, txt_w);

    const int scaled_digit_w = DIGIT_WIDTH * txt_scale;
    const int scaled_txt_w   = txt_len * scaled_digit_w;
    const int scaled_txt_h   = DIGIT_HEIGHT * txt_scale;

    int x = (win_w / 2) - (scaled_txt_w / 2);
    int y = (win_h / 2) - (scaled_txt_h / 2);
    for (int i = 0; i < txt_len; i++) {
        int digit_idx = char_to_idx(str[i]);
        if (digit_idx < 0)
            continue;

        draw_digit(rend, texture, x, y, txt_scale, digit_idx);

        x += scaled_digit_w;
    }
}

/*----------------------------------------------------------------------------*/

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        die("Unable to start SDL.");

    const int window_w = DIGIT_WIDTH * (sizeof("00:00:00") - 1);
    const int window_h = DIGIT_HEIGHT;

    /* Create SDL window */
    sdl_window =
      SDL_CreateWindow("sclock", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_w, window_h, SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL)
        die("Error creating SDL window.");

    /* Create SDL renderer */
    SDL_Renderer* sdl_renderer =
      SDL_CreateRenderer(sdl_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL)
        die("Error creating SDL renderer.");

    /* Use the best scaling quality of the texture */
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") != SDL_TRUE)
        die("Could not set RENDER_SCALE_QUALITY hint.");

    /* Create the surface and texture for the digits */
    SDL_Surface* digits_surface =
      SDL_CreateRGBSurfaceFrom(digits.pixel_data, digits.width, digits.height,
                               digits.bytes_per_pixel * 8,
                               digits.width * digits.bytes_per_pixel,
                               0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    if (!digits_surface)
        die("Error creating RGB surface from PNG data.");

    SDL_Texture* digits_texture =
      SDL_CreateTextureFromSurface(sdl_renderer, digits_surface);
    if (!digits_texture)
        die("Error creating texture from RGB surface.");

    static char time_str[]   = "00:00:00";
    static char title_str[]  = "00:00:00 - sclock";
    static char prev_title[] = "00:00:00 - sclock";

    /* Main loop */
    bool running = true;
    while (running) {
        /* Parse SDL events */
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT: {
                    running = false;
                } break;

                case SDL_KEYDOWN: {
                    switch (sdl_event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                        case SDL_SCANCODE_Q: {
                            running = false;
                        } break;

                        default:
                            break;
                    }
                } break;

                default:
                    break;
            }
        }

        /* Clear window */
        set_render_color(sdl_renderer, palettes[palette][COLOR_BACKGROUND]);
        SDL_RenderClear(sdl_renderer);

        /* TODO: Draw background texture */

        /* TODO: Add modes */
        /* TODO: Change colors depending on mode status, etc. */
        set_texture_color(digits_texture, palettes[palette][COLOR_FOREGROUND]);

        time_t t      = time(NULL);
        struct tm* tm = localtime(&t);
        int hours     = tm->tm_hour;
        int minutes   = tm->tm_min;
        int seconds   = tm->tm_sec;
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", hours, minutes,
                 seconds);

        draw_string(sdl_renderer, digits_texture, time_str);

        /* Change window title to show time */
        snprintf(title_str, sizeof(title_str), "%02d:%02d:%02d - sclock", hours,
                 minutes, seconds);
        if (strcmp(title_str, prev_title) != 0)
            SDL_SetWindowTitle(sdl_window, title_str);
        memcpy(title_str, prev_title, sizeof(title_str));

        /* Send to renderer and delay depending on FPS */
        SDL_RenderPresent(sdl_renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}
