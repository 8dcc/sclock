
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define WINDOW_W 640
#define WINDOW_H 480
#define FPS      60

static void die(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    vfprintf(stderr, fmt, va);
    putc('\n', stderr);

    SDL_Quit();
    exit(1);
}

static inline void set_render_color(SDL_Renderer* rend, uint32_t col) {
    const uint8_t r = (col >> 16) & 0xFF;
    const uint8_t g = (col >> 8) & 0xFF;
    const uint8_t b = (col >> 0) & 0xFF;
    const uint8_t a = 255;
    SDL_SetRenderDrawColor(rend, r, g, b, a);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        die("Unable to start SDL.");

    /* Create SDL window */
    SDL_Window* sdl_window =
      SDL_CreateWindow("TODO: Title", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);
    if (!sdl_window)
        die("Error creating SDL window.");

    /* Create SDL renderer */
    SDL_Renderer* sdl_renderer =
      SDL_CreateRenderer(sdl_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
        SDL_DestroyWindow(sdl_window);
        die("Error creating SDL renderer.");
    }

    /* Main loop */
    bool running = true;
    while (running) {
        /* Parse SDL events */
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (sdl_event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                        case SDL_SCANCODE_Q:
                            running = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        /* Clear window */
        set_render_color(sdl_renderer, 0x000000);
        SDL_RenderClear(sdl_renderer);

        /* TODO */

        /* Send to renderer and delay depending on FPS */
        SDL_RenderPresent(sdl_renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}
