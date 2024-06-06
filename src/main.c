
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>

#define FPS       60
#define MAX_TITLE 255

#include "types.h"
#include "config.h" /* palettes, max_digit_scale, etc. */
#include "digits.h"

/*----------------------------------------------------------------------------*/
/* Globals */

static SDL_Window* g_window          = NULL;
static SDL_Renderer* g_renderer      = NULL;
static SDL_Texture* g_digits_texture = NULL;

static bool g_draw_grid   = true;
static EPalette g_palette = default_palette;

/*----------------------------------------------------------------------------*/
/* Misc helper functions */

static void die(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    vfprintf(stderr, fmt, va);
    putc('\n', stderr);

    if (g_window != NULL)
        SDL_DestroyWindow(g_window);

    SDL_Quit();
    exit(1);
}

static unsigned long time_in_seconds(void) {
    /* Get the current time */
    time_t t      = time(NULL);
    struct tm* tm = localtime(&t);

    /* Convert to seconds */
    unsigned long result = 0;
    result += tm->tm_hour * 60 * 60;
    result += tm->tm_min * 60;
    result += tm->tm_sec;
    return result;
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

static void draw_grid(void) {
    if (!g_draw_grid || grid_step == 0)
        return;

    int win_w, win_h;
    SDL_GetWindowSize(g_window, &win_w, &win_h);

    const int step = grid_step + 1;
    for (int y = grid_step; y < win_h; y += step)
        for (int x = grid_step; x < win_w; x += step)
            SDL_RenderDrawPoint(g_renderer, x, y);
}

static void draw_digit(int x, int y, float scale, int digit_idx) {
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

    SDL_RenderCopy(g_renderer, g_digits_texture, &src_rect, &dst_rect);
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

/* Return the scale for the text size, depending on it's width, and the window
 * size. */
static float get_text_scale(int win_w, int win_h, int txt_w) {
    float aspect_ratio_txt = (float)txt_w / (float)DIGIT_HEIGHT;
    float aspect_ratio_win = (float)win_w / (float)win_h;

    float scale = (aspect_ratio_txt > aspect_ratio_win)
                    ? (float)win_w / (float)txt_w
                    : (float)win_h / (float)DIGIT_HEIGHT;

    return (scale < max_digit_scale) ? scale : max_digit_scale;
}

static void draw_string(const char* str) {
    int win_w, win_h;
    SDL_GetWindowSize(g_window, &win_w, &win_h);

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

        draw_digit(x, y, txt_scale, digit_idx);

        x += scaled_digit_w;
    }
}

static void draw_time(int hours, int minutes, int seconds) {
    static char time_str[] = "00:00:00";

    if (hours > 99)
        hours = 99;
    else if (hours < 0)
        hours = 0;

    if (minutes > 59)
        minutes = 59;
    else if (minutes < 0)
        minutes = 0;

    if (seconds > 59)
        seconds = 59;
    else if (seconds < 0)
        seconds = 0;

    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", hours, minutes,
             seconds);

    draw_string(time_str);
}

/*----------------------------------------------------------------------------*/

int main(int argc, char** argv) {
    EMode mode            = MODE_CLOCK;
    bool stopwatch_paused = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "clock"))
            mode = MODE_CLOCK;
        else if (!strcmp(argv[i], "stopwatch"))
            mode = MODE_STOPWATCH;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        die("Unable to start SDL.");

    const int window_w = DIGIT_WIDTH * (sizeof("00:00:00") - 1);
    const int window_h = DIGIT_HEIGHT;

    /* Create SDL window */
    g_window =
      SDL_CreateWindow("sclock", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_w, window_h, SDL_WINDOW_RESIZABLE);
    if (g_window == NULL)
        die("Error creating SDL window.");

    /* Create SDL renderer */
    g_renderer =
      SDL_CreateRenderer(g_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (g_renderer == NULL)
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

    g_digits_texture = SDL_CreateTextureFromSurface(g_renderer, digits_surface);
    if (!g_digits_texture)
        die("Error creating texture from RGB surface.");

    /* Time when we drawed the last frame, but not necessarily the content we
     * drawed. Used to check the passed time independently of runtime
     * performance. */
    unsigned long last_time = time_in_seconds();

    /* Time in seconds that we actually rendered in the last frame. Used for
     * allowing pauses in the stopwatch, for example. */
    unsigned long last_displayed = (mode == MODE_CLOCK)       ? last_time
                                   : (mode == MODE_STOPWATCH) ? 0
                                                              : 0;

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

                        case SDL_SCANCODE_G: {
                            g_draw_grid = !g_draw_grid;
                        } break;

                        case SDL_SCANCODE_T:
                        case SDL_SCANCODE_P: {
                            /* Cycle color palettes/themes */
                            g_palette++;
                            if (g_palette >= PALETTE_COUNT)
                                g_palette = 0;
                        } break;

                        case SDL_SCANCODE_F11:
                        case SDL_SCANCODE_F: {
                            /* Toggle fullscreen */
                            uint32_t new_flags =
                              (SDL_GetWindowFlags(g_window) &
                               SDL_WINDOW_FULLSCREEN_DESKTOP)
                                ? 0
                                : SDL_WINDOW_FULLSCREEN_DESKTOP;

                            SDL_SetWindowFullscreen(g_window, new_flags);
                        } break;

                        case SDL_SCANCODE_SPACE: {
                            if (mode == MODE_STOPWATCH)
                                stopwatch_paused = !stopwatch_paused;
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
        set_render_color(g_renderer, palettes[g_palette][COLOR_BACKGROUND]);
        SDL_RenderClear(g_renderer);

        /* Draw background texture */
        set_render_color(g_renderer, palettes[g_palette][COLOR_GRID]);
        draw_grid();

        unsigned long now            = time_in_seconds();
        unsigned long displayed_time = 0;

        switch (mode) {
            case MODE_CLOCK: {
                set_texture_color(g_digits_texture,
                                  palettes[g_palette][COLOR_FOREGROUND]);

                displayed_time = now;
            } break;

            case MODE_STOPWATCH: {
                /* Change foreground color depending on status */
                uint32_t color = (stopwatch_paused)
                                   ? palettes[g_palette][COLOR_PAUSED]
                                   : palettes[g_palette][COLOR_FOREGROUND];
                set_texture_color(g_digits_texture, color);

                /* Time has passed */
                if (!stopwatch_paused && now > last_time)
                    displayed_time = last_displayed + now - last_time;
                else
                    displayed_time = last_displayed;
            } break;

            default:
                break;
        }

        /* Convert the time and render it */
        unsigned int h = displayed_time / (60 * 60);
        unsigned int m = displayed_time / 60 % 60;
        unsigned int s = displayed_time % 60;
        draw_time(h, m, s);

        /* Save the current time and the time we actually rendered. */
        last_time      = now;
        last_displayed = displayed_time;

        /* Status for the window title */
        const char* status_str =
          (mode == MODE_STOPWATCH && stopwatch_paused) ? "(paused)" : "";

        /* Save the window title */
        static char title_str[MAX_TITLE] = "";
        snprintf(title_str, MAX_TITLE, "%02d:%02d:%02d - sclock %s", h, m, s,
                 status_str);

        /* If the title changed, update it */
        static char prev_title[MAX_TITLE] = "";
        if (strcmp(title_str, prev_title) != 0)
            SDL_SetWindowTitle(g_window, title_str);
        memcpy(title_str, prev_title, sizeof(title_str));

        /* Send to renderer and delay depending on FPS */
        SDL_RenderPresent(g_renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
