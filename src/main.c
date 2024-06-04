
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define WINDOW_W 640
#define WINDOW_H 480
#define FPS      60

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
                       int digit_idx) {
    const SDL_Rect src_rect = {
        digit_idx * DIGIT_WIDTH,
        0,
        DIGIT_WIDTH,
        digits.height,
    };
    const SDL_Rect dst_rect = {
        x,
        y,
        DIGIT_WIDTH,
        digits.height,
    };
    SDL_RenderCopy(rend, texture, &src_rect, &dst_rect);
}

/*----------------------------------------------------------------------------*/

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        die("Unable to start SDL.");

    /* Create SDL window */
    sdl_window =
      SDL_CreateWindow("sclock", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL)
        die("Error creating SDL window.");

    /* Create SDL renderer */
    SDL_Renderer* sdl_renderer =
      SDL_CreateRenderer(sdl_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL)
        die("Error creating SDL renderer.");

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

        /* TODO: Change colors depending on more conditions */
        set_texture_color(digits_texture, palettes[palette][COLOR_FOREGROUND]);

        /*
         * TODO: Calculate width and height of digits depending on the size and
         * aspect ratio of the window.
         * TODO: Calulate positions dinamically depending on width of previous
         * character.
         * TODO: Draw actual time
         */
        draw_digit(sdl_renderer, digits_texture, 30, 30, 7);

#if 0
        uint64_t hours = time / 60 / 60;
        uint64_t minutes = time / 60 % 60;
        uint64_t seconds = time % 60;
#endif

        /* Send to renderer and delay depending on FPS */
        SDL_RenderPresent(sdl_renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}
