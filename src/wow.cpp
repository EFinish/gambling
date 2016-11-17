#include "../include/SDL.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

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

#define COLUMN_SIZE 10
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#define SLOT_SIZE 128
#define SLOT_PADDING 16

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define BOARD_SIZE ((SLOT_SIZE + SLOT_PADDING) * 3)

struct Slot {
    SDL_Texture* texture;
	u32 points;
};

struct Board_Slot {
	Slot* slot = NULL;
	i32 x = 0;
	i32 y = 0;
};

SDL_Texture* load_texture(SDL_Renderer* renderer, char* file_name) {
	SDL_Texture* result = NULL;

	SDL_Surface* surface = SDL_LoadBMP(file_name);
	assert(surface != NULL);

	result = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	return result;
}

Board_Slot* get_previous_slot(Board_Slot* slots, u32 slots_count, u32 slot_index) {
    assert(slot_index >= 0 && slot_index < slots_count);

    Board_Slot* result = NULL;
    
    if ((i32)slot_index - 1 < 0) {
        result = &slots[slots_count - 1];
    }
    else {
        result = &slots[slot_index - 1];
    }
    
    return result;
}

Board_Slot* get_next_slot(Board_Slot* slots, u32 slots_count, u32 slot_index) {
    assert(slot_index >= 0 && slot_index < slots_count);
    
    Board_Slot* result = NULL;
    
    if (slot_index + 1 >= slots_count) {
        result = &slots[0];
    }
    else {
        result = &slots[slot_index + 1];
    }
    
    return result;
}

i32 main(i32 argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"Gambling", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		SDL_WINDOW_OPENGL);
		
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Surface* icon = SDL_LoadBMP("osterzzzz.bmp");
	assert(icon != NULL);

	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

    printf("Slot machine started\n");
    srand(time(NULL));
        
    Slot slots[] = {
        { 
			load_texture(renderer, "bell.bmp"), 
			35
		},
        { 
			load_texture(renderer, "cherry.bmp"),
			20
		},
        { 
			load_texture(renderer, "jewel.bmp"),
			85
		},
        { 
			load_texture(renderer, "orange.bmp"),
			50 
		},
        { 
			load_texture(renderer, "seven.bmp"),
			100
		},
        { 
			load_texture(renderer, "singleBar.bmp"),
			2
		},
        { 
			load_texture(renderer, "tripleBar.bmp"),
			10
		}
    };
    
	// Reel reels[REEL_COUNT];

	// generate_reels(reels, REEL_COUNT);
	// pull_reels(reels, REEL_COUNT);

	// for (u32 i = 0; i < REEL_COUNT; i++) {
	// 	draw_symbol(reels[i].symbol, reels[i].position);
	// }

    Board_Slot column_1[COLUMN_SIZE];
    Board_Slot column_2[COLUMN_SIZE];
    Board_Slot column_3[COLUMN_SIZE];
    
    for (u32 i = 0; i < COLUMN_SIZE; i++) {    
        column_1[i].slot = &slots[rand() % array_count(slots)];
        column_2[i].slot = &slots[rand() % array_count(slots)];
        column_3[i].slot = &slots[rand() % array_count(slots)];
    }

    u32 score = 0;
	r32 play_timer = 0.0f;
    
	Board_Slot* board[3][3];
	bool won_horizontal = false;

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

		SDL_RenderClear(renderer);

		if ((play_timer -= delta_time) <= 0.0f) {
			u32 column_1_index = rand() % COLUMN_SIZE;
			u32 column_2_index = rand() % COLUMN_SIZE;
			u32 column_3_index = rand() % COLUMN_SIZE;

			board[0][0] = get_previous_slot(column_1, COLUMN_SIZE, column_1_index);
			board[0][1] = &column_1[column_1_index];
			board[0][2] = get_next_slot(column_1, COLUMN_SIZE, column_1_index);

			board[0][0]->x = 0;
			board[0][0]->y = 0;

			board[0][1]->x = 0;
			board[0][1]->y = SLOT_SIZE + SLOT_PADDING;
			
			board[0][2]->x = 0;
			board[0][2]->y = board[0][1]->y + SLOT_SIZE + SLOT_PADDING;

			board[1][0] = get_previous_slot(column_2, COLUMN_SIZE, column_2_index);
			board[1][1] = &column_2[column_2_index];
			board[1][2] = get_next_slot(column_2, COLUMN_SIZE, column_2_index);

			board[1][0]->x = SLOT_SIZE + SLOT_PADDING;
			board[1][0]->y = 0;

			board[1][1]->x = SLOT_SIZE + SLOT_PADDING;
			board[1][1]->y = SLOT_SIZE + SLOT_PADDING;

			board[1][2]->x = SLOT_SIZE + SLOT_PADDING;
			board[1][2]->y = board[1][1]->y + SLOT_SIZE + SLOT_PADDING;

			board[2][0] = get_previous_slot(column_3, COLUMN_SIZE, column_3_index);
			board[2][1] = &column_3[column_3_index];
			board[2][2] = get_next_slot(column_3, COLUMN_SIZE, column_3_index);

			board[2][0]->x = board[1][0]->x + SLOT_SIZE + SLOT_PADDING;
			board[2][0]->y = 0;
			
			board[2][1]->x = board[1][0]->x + SLOT_SIZE + SLOT_PADDING;
			board[2][1]->y = SLOT_SIZE + SLOT_PADDING;
			
			board[2][2]->x = board[1][0]->x + SLOT_SIZE + SLOT_PADDING;
			board[2][2]->y = board[2][1]->y + SLOT_SIZE + SLOT_PADDING;

			for (u32 i = 0; i < 3; i++) {
				for (u32 j = 0; j < 3; j++) {
					board[i][j]->x += (WINDOW_WIDTH / 2) - (BOARD_SIZE / 2);
					board[i][j]->y += (WINDOW_HEIGHT / 2) - (BOARD_SIZE / 2);
				}
			}

			if (board[0][1]->slot == board[1][1]->slot && board[0][1]->slot == board[2][1]->slot) {
				won_horizontal = true;
				score += board[0][1]->slot->points;
			}
			else {
				won_horizontal = false;
			}

			if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
				score += board[0][0]->slot->points;
			}

			if (board[0][2] == board[1][1] && board[2][0] == board[0][2]) {
				score += board[2][0]->slot->points;
			}

			play_timer = 1.0f;
		}
        
        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
				Board_Slot* bs = board[i][j];

                SDL_Rect rect = { 
					bs->x, 
					bs->y, 
					SLOT_SIZE, 
					SLOT_SIZE
				};

				SDL_RenderCopy(renderer, bs->slot->texture, NULL, &rect);
            }
        }
		
		if (won_horizontal) {
			SDL_RenderDrawLine(renderer, 
				(WINDOW_WIDTH / 2) - (SLOT_SIZE + SLOT_PADDING),
				WINDOW_HEIGHT / 2,
				(WINDOW_WIDTH / 2) + (SLOT_SIZE + SLOT_PADDING),
				WINDOW_HEIGHT / 2);
		}

		SDL_RenderPresent(renderer);
    }
    
	return 0;
}
