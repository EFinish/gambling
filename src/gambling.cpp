#include "../lib/sdl/sdl.h"

#include <stdlib.h>
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
	i32 scroll_offset = 0;

	Symbol* symbols[REEL_SYMBOLS_COUNT];
	u32 index = 0;
};

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

	Symbol symbols[SYMBOL_COUNT];

	symbols[0] = symbol("bell.bmp", 35);
	symbols[1] = symbol("cherry.bmp", 20);
	symbols[2] = symbol("jewel.bmp", 85);
	symbols[3] = symbol("orange.bmp", 50);
	symbols[4] = symbol("seven.bmp", 100);
	symbols[5] = symbol("singleBar.bmp", 2);
	symbols[6] = symbol("tripleBar.bmp", 10);
    
	Reel reels[REEL_COUNT];

	i32 base_y = (WINDOW_HEIGHT / 2) - ((SYMBOL_SIZE / 2) + SYMBOL_SIZE + SYMBOL_PADDING);

	reels[0].base_x = (WINDOW_WIDTH / 2) - ((SYMBOL_SIZE / 2) + SYMBOL_SIZE + SYMBOL_PADDING);
	reels[0].base_y = base_y;
	reels[1].base_x = reels[0].base_x + SYMBOL_SIZE + SYMBOL_PADDING;
	reels[1].base_y = base_y;
	reels[2].base_x = reels[1].base_x + SYMBOL_SIZE + SYMBOL_PADDING;
	reels[2].base_y = base_y;
	
    u32 score = 0;
	r32 play_timer = 0.0f;
	bool won = false;

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
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if ((play_timer -= delta_time) <= 0.0f) {
			for (u32 i = 0; i < REEL_COUNT; i++) {
				Reel* reel = &reels[i];

				for (u32 j = 0; j < REEL_SYMBOLS_COUNT; j++) {
					i32 symbol_index = rand() % SYMBOL_COUNT;
					reel->symbols[j] = &symbols[symbol_index];
					// reel->symbols[j]->scroll_offset = (j * SYMBOL_SIZE) + SYMBOL_PADDING;
				}
			}

			// if (board[0][1]->slot == board[1][1]->slot && board[0][1]->slot == board[2][1]->slot) {
			// 	won = true;
			// 	score += board[0][1]->slot->points;
			// }
			// else {
			// 	won = false;
			// }

			// if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
			// 	score += board[0][0]->slot->points;
			// }

			// if (board[0][2] == board[1][1] && board[2][0] == board[0][2]) {
			// 	score += board[2][0]->slot->points;
			// }

			play_timer = 1.0f;
		}
		for (u32 i = 0; i < REEL_COUNT; i++) {
			Reel* reel = &reels[i];

			for (u32 j = 0; j < REEL_SYMBOLS_COUNT; j++) {
				Symbol* symbol = reel->symbols[j];

				SDL_Rect symbol_rect; 

				symbol_rect.x = reel->base_x; 
				symbol_rect.w = SYMBOL_SIZE; 
				symbol_rect.h = SYMBOL_SIZE;
				//Added code here for checking if reached limits of window
				if(reel->base_y + symbol->scroll_offset > WINDOW_HEIGHT){
					symbol->scroll_offset = 0;
				}
				symbol_rect.y = reel->base_y + symbol->scroll_offset++;

				SDL_RenderCopy(renderer, symbol->texture, NULL, &symbol_rect);
			}

			SDL_Rect base_rect;
			base_rect.x = reel->base_x - 8;
			base_rect.y = reel->base_y - 8;
			base_rect.w = 16;
			base_rect.h = 16;

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderFillRect(renderer, &base_rect);

			// reel->scroll_offset++;
			// if (reel->scroll_offset > 150.0f) {
			// 	reel->scroll_offset = 0;
			// }
		}

		// if (won) {
		// 	SDL_RenderDrawLine(renderer, 
		// 		(WINDOW_WIDTH / 2) - (SLOT_SIZE + SLOT_PADDING),
		// 		WINDOW_HEIGHT / 2,
		// 		(WINDOW_WIDTH / 2) + (SLOT_SIZE + SLOT_PADDING),
		// 		WINDOW_HEIGHT / 2);
		// }

		SDL_RenderPresent(renderer);
    }
    
	return 0;
}
