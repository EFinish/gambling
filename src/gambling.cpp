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

struct Texture {
    u32 width;
    u32 height;

    SDL_Texture* handle;
};

Texture make_texture(char* file_name) {
    Texture texture;

    SDL_Surface* surface = SDL_LoadBMP(file_name);
    
    u32 key = SDL_MapRGB(surface->format, 255, 0, 255);
    SDL_SetColorKey(surface, SDL_TRUE, key);

    texture.width = surface->w;
    texture.height = surface->h;

    texture.handle = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

void draw_texture(Texture* texture, i32 x, i32 y, i32 w, i32 h) {
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderCopy(renderer, texture->handle, NULL, &rect);
}

void draw_texture(Texture* texture, i32 x, i32 y) {
    draw_texture(texture, x, y, (i32) texture->width, (i32) texture->height);
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
    // if (text->texture) {
    //     SDL_DestroyTexture(text->texture);
    // }

    SDL_Surface* surface = TTF_RenderText_Blended(font, value, { 255, 255, 255, 255 });

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

void draw_text(Text* text) {
    SDL_assert(text->texture != NULL);
    SDL_RenderCopy(renderer, text->texture, NULL, &text->rect);
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
    
    font = TTF_OpenFont("RioGrande.ttf", 42);
    SDL_assert(font != NULL);

    Texture background_texture = make_texture("overlay.bmp");
    Text score_text = make_text(90, (WINDOW_HEIGHT / 2) + 90, "00000000");

    Symbol symbols[SYMBOL_COUNT];

    symbols[0] = symbol("slot_bell.bmp", 35);
    symbols[1] = symbol("slot_cherry.bmp", 20);
    symbols[2] = symbol("slot_jewel.bmp", 85);
    symbols[3] = symbol("slot_orange.bmp", 50);
    symbols[4] = symbol("slot_seven.bmp", 100);
    symbols[5] = symbol("slot_singleBar.bmp", 2);
    symbols[6] = symbol("slot_tripleBar.bmp", 10);
    
    Reel reels[REEL_COUNT];

    i32 base_y = -(SYMBOL_SIZE + SYMBOL_PADDING);

    reels[0].base_x = WINDOW_WIDTH - ((SYMBOL_SIZE + SYMBOL_PADDING) * 3) - 90;
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
                        reels[i].scroll_velocity = random_between(2500.0f, 5000.0f);
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

                r64 _;
                symbol_rect.y = reel->base_y + (modf(offset, &_) * (SYMBOL_SIZE + SYMBOL_PADDING)) + (l * (SYMBOL_SIZE + SYMBOL_PADDING));

                symbol_rect.w = SYMBOL_SIZE;
                symbol_rect.h = SYMBOL_SIZE;

                SDL_RenderCopy(renderer, symbol->texture, NULL, &symbol_rect);
            }

            reel->scroll_offset += reel->scroll_velocity * delta_time;
            reel->scroll_velocity -= (1.0f * reel->scroll_velocity) * delta_time;

            if (reel->scroll_offset > REEL_SYMBOLS_COUNT * (SYMBOL_SIZE + SYMBOL_PADDING)) {
                reel->scroll_offset = 0.0f;
            }

            if (reel->scroll_velocity > 0.0f && reel->scroll_velocity < 1000.0f) {
                r32 offset = reel->scroll_offset / (SYMBOL_SIZE + SYMBOL_PADDING);
                
                r64 _;
                r64 fraction = modf(offset, &_);

                if (fraction > 0.9) {
                    reel->scroll_offset = ceilf(offset) * (SYMBOL_SIZE + SYMBOL_PADDING);
                    reel->scroll_velocity = 0.0f;
                }
                else if (fraction < 0.1) {
                    reel->scroll_offset = floorf(offset) * (SYMBOL_SIZE + SYMBOL_PADDING);
                    reel->scroll_velocity = 0.0f;
                }
            }
        }

        draw_texture(&background_texture, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        SDL_Rect rect;
        rect.x = (WINDOW_WIDTH / 2) - 215;
        rect.y = (WINDOW_HEIGHT / 2) + 30;

        rect.w = 240;
        rect.h = 90;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        if (is_spinning) {
            bool all_have_stopped = true;
            for (u32 i = 0; i < REEL_COUNT; i++) {
                Reel* reel = &reels[i];

                if (reel->scroll_velocity > 10.0f) {
                    all_have_stopped = false;
                }
            }

            if (all_have_stopped) {
                // @todo(Ryan): Check for win conditions

                char buffer[64];
                sprintf(buffer, "%.8u", score += 10);

                change_text(&score_text, buffer);

                is_spinning = false;
            }
        }

        draw_text(&score_text);

        SDL_RenderPresent(renderer);
    }
    
    return 0;
}
