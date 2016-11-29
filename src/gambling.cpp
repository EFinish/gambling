#include "../lib/sdl/sdl.h"
#include "../lib/sdl/sdl_ttf.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float r32;
typedef double r64;

#define array_count(array) (sizeof(array) / sizeof(array[0]))

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

SDL_Window* window;
SDL_Renderer* renderer;

r32 lerp(r32 a, r32 t, r32 b) {
    return (1.0f - t) * a + t * b;
}

i32 random_choice(i32 c) {
    return rand() % c;
}

r32 random_between(r32 min, r32 max) {
    return lerp(min, (r32) rand() * (1.0f / (r32) RAND_MAX), max);
}

#define SYMBOL_COUNT 7
#define SYMBOL_SIZE 128
#define SYMBOL_PADDING 16

struct Symbol {
    SDL_Texture* texture = NULL;
    u32 points = 0;
    i32 scroll_offset = 0;
};

Symbol symbol(char* file_name, u32 points) {
    Symbol result;
    result.points = points;
    result.scroll_offset = 0;

    SDL_Surface* surface = SDL_LoadBMP(file_name);
    SDL_assert(surface != NULL);

    result.texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return result;
}

#define REEL_COUNT 3
#define REEL_SYMBOLS_COUNT 7

struct Reel {
    i32 base_x = 0;
    i32 base_y = 0;

    Symbol* symbols[REEL_SYMBOLS_COUNT];

    r32 scroll_offset = 0.0f;
    r32 scroll_velocity = 0.0f;
};

TTF_Font* font;

struct Text {
    SDL_Texture* texture;
    SDL_Rect rect;
};

void change_text(Text* text, char* value) {
    if (text->texture) {
        SDL_DestroyTexture(text->texture);
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, value, { 255, 255, 255, 255 });

    text->rect.w = surface->w;
    text->rect.h = surface->h;

    text->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

Text make_text(i32 x, i32 y, char* value) {
    Text text;

    text.rect.x = x;
    text.rect.y = y;

    change_text(&text, value);

    return text;
}

i32 main(i32 argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));

    window = SDL_CreateWindow(
        "Gambling", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        SDL_WINDOW_OPENGL);

    SDL_Surface* icon = SDL_LoadBMP("oster.bmp");
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Init();
    
    font = TTF_OpenFont("c:/windows/fonts/arial.ttf", 24);
    SDL_assert(font != NULL);

    Text score_text = make_text(10, 10, "0");

    Symbol symbols[SYMBOL_COUNT];

    symbols[0] = symbol("bell.bmp", 35);
    symbols[1] = symbol("cherry.bmp", 20);
    symbols[2] = symbol("jewel.bmp", 85);
    symbols[3] = symbol("orange.bmp", 50);
    symbols[4] = symbol("seven.bmp", 100);
    symbols[5] = symbol("singleBar.bmp", 2);
    symbols[6] = symbol("tripleBar.bmp", 10);
    
    Reel reels[REEL_COUNT];

    i32 base_y = -(SYMBOL_SIZE + SYMBOL_PADDING);

    reels[0].base_x = (WINDOW_WIDTH / 2) - ((SYMBOL_SIZE / 2) + SYMBOL_SIZE + SYMBOL_PADDING);
    reels[0].base_y = base_y;
    reels[1].base_x = reels[0].base_x + SYMBOL_SIZE + SYMBOL_PADDING;
    reels[1].base_y = base_y;
    reels[2].base_x = reels[1].base_x + SYMBOL_SIZE + SYMBOL_PADDING;
    reels[2].base_y = base_y;
    
    u32 score = 0;
    
    bool won = false;
    bool is_spinning = false;

    for (u32 i = 0; i < REEL_COUNT; i++) {
        Reel* reel = &reels[i];

        for (u32 j = 0; j < REEL_SYMBOLS_COUNT; j++) {
            i32 symbol_index = random_choice(SYMBOL_COUNT);
            reel->symbols[j] = &symbols[symbol_index];
        }
    }

    u64 timer_frequency = SDL_GetPerformanceFrequency();
    u64 last_time = SDL_GetPerformanceCounter();

    bool is_running = true;
    while (is_running) {
        u64 current_time = SDL_GetPerformanceCounter();
        
        r32 delta_time = (r32)(current_time - last_time) / (r32)timer_frequency;
        last_time = current_time;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }
                else if (event.key.keysym.sym == SDLK_w) {
                    for (u32 i = 0; i < REEL_COUNT; i++) {
                        Reel* reel = &reels[i];

                        for (u32 j = 0; j < REEL_SYMBOLS_COUNT; j++) {
                            i32 symbol_index = random_choice(SYMBOL_COUNT);
                            reel->symbols[j] = &symbols[symbol_index];
                        }
                    }
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    for (u32 i = 0; i < REEL_COUNT; i++) {
                        reels[i].scroll_velocity = (random_choice(3) + 5) * 1000.0f;
                        is_spinning = true;
                    }
                }
            }
        }

        if (!is_running) {
            break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (u32 i = 0; i < REEL_COUNT; i++) {
            Reel* reel = &reels[i];

            r32 offset = reel->scroll_offset / (SYMBOL_SIZE + SYMBOL_PADDING);
            i32 j = (i32) offset;

            for (u32 k = 0; k < REEL_SYMBOLS_COUNT; k++) {
                u32 l = 0;
                if (j + k < REEL_SYMBOLS_COUNT) {
                    l = j + k;
                }
                else {
                    l = j + k - REEL_SYMBOLS_COUNT;
                }

                Symbol* symbol = reel->symbols[k];
                SDL_Rect symbol_rect;

                symbol_rect.x = reel->base_x;

                r64 blegh;
                symbol_rect.y = reel->base_y + (modf(offset, &blegh) * (SYMBOL_SIZE + SYMBOL_PADDING)) + (l * (SYMBOL_SIZE + SYMBOL_PADDING));
                symbol_rect.w = SYMBOL_SIZE;
                symbol_rect.h = SYMBOL_SIZE;

                SDL_RenderCopy(renderer, symbol->texture, NULL, &symbol_rect);
            }

            reel->scroll_offset += reel->scroll_velocity * delta_time;
            reel->scroll_velocity -= (2.0f * reel->scroll_velocity) * delta_time;

            if (reel->scroll_offset > REEL_SYMBOLS_COUNT * (SYMBOL_SIZE + SYMBOL_PADDING)) {
                reel->scroll_offset = 0.0f;
            }
        }

        if (is_spinning) {
            bool all_have_stopped = true;
            for (u32 i = 0; i < REEL_COUNT; i++) {
                Reel* reel = &reels[i];

                if (reel->scroll_velocity > 10.0f) {
                    all_have_stopped = false;
                }
                else {
                    reel->scroll_velocity = 0.0f;
                }
            }

            if (all_have_stopped) {
                // @todo(Ryan): Check for win conditions

                char buffer[64];
                sprintf(buffer, "%u", score += 10);

                change_text(&score_text, buffer);

                is_spinning = false;
            }
        }

        SDL_RenderCopy(renderer, score_text.texture, NULL, &score_text.rect);
        SDL_RenderPresent(renderer);
    }
    
    return 0;
}
