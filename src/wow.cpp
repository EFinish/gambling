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

struct v2 {
    i32 x;
    i32 y;
};

struct Slot {
    SDL_Texture* texture;
	u32 points;
	
	v2 position;
};

void draw_slot(Slot* slot, SDL_Renderer* renderer) {
    SDL_Rect rect = { 
        slot->position.x, 
        slot->position.y, 
        SLOT_SIZE, 
		SLOT_SIZE
    };

    SDL_RenderCopy(renderer, slot->texture, NULL, &rect);
}

SDL_Texture* load_texture(SDL_Renderer* renderer, char* file_name) {
	SDL_Texture* result = NULL;

	SDL_Surface* surface = SDL_LoadBMP(file_name);
	assert(surface != NULL);

	result = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	return result;
}

Slot* get_previous_slot(Slot** slots, u32 slots_count, u32 slot_index) {
    assert(slot_index >= 0 && slot_index < slots_count);

    Slot* result = NULL;
    
    if ((i32)slot_index - 1 < 0) {
        result = slots[slots_count - 1];
    }
    else {
        result = slots[slot_index - 1];
    }
    
    return result;
}

Slot* get_next_slot(Slot** slots, u32 slots_count, u32 slot_index) {
    assert(slot_index >= 0 && slot_index < slots_count);
    
    Slot* result = NULL;
    
    if (slot_index + 1 >= slots_count) {
        result = slots[0];
    }
    else {
        result = slots[slot_index + 1];
    }
    
    return result;
}

i32 main(i32 argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Gambling", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 720, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    printf("Slot machine started\n");
    srand(time(NULL));
        
    Slot slots[] = {
        { 
			load_texture(renderer, "../assets/bell.bmp"), 
			35
		},
        { 
			load_texture(renderer, "../assets/cherry.bmp"),
			20
		},
        { 
			load_texture(renderer, "../assets/jewel.bmp"),
			85
		},
        { 
			load_texture(renderer, "../assets/orange.bmp"),
			50 
		},
        { 
			load_texture(renderer, "../assets/seven.bmp"),
			100
		},
        { 
			load_texture(renderer, "../assets/singleBar.bmp"),
			2
		},
        { 
			load_texture(renderer, "../assets/tripleBar.bmp"),
			10
		}
    };
    
    Slot* column_1[COLUMN_SIZE];
    Slot* column_2[COLUMN_SIZE];
    Slot* column_3[COLUMN_SIZE];
    
    for (u32 i = 0; i < COLUMN_SIZE; i++) {    
        column_1[i] = &slots[rand() % array_count(slots)];
        column_2[i] = &slots[rand() % array_count(slots)];
        column_3[i] = &slots[rand() % array_count(slots)];
    }

    u32 score = 0;
	r32 play_timer = 0.0f;
    
	Slot* board[3][3] = {};

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
			board[0][1] = column_1[column_1_index];
			board[0][2] = get_next_slot(column_1, COLUMN_SIZE, column_1_index);

			board[0][0]->position = { 0, 0 };
			board[0][1]->position = { 0, SLOT_SIZE };
			board[0][2]->position = { 0, SLOT_SIZE * 2 };

			board[1][0] = get_previous_slot(column_2, COLUMN_SIZE, column_2_index);
			board[1][1] = column_2[column_2_index];
			board[1][2] = get_next_slot(column_2, COLUMN_SIZE, column_2_index);

			board[1][0]->position = { SLOT_SIZE, 0 };
			board[1][1]->position = { SLOT_SIZE, SLOT_SIZE };
			board[1][2]->position = { SLOT_SIZE, SLOT_SIZE * 2 };

			board[2][0] = get_previous_slot(column_3, COLUMN_SIZE, column_3_index);
			board[2][1] = column_3[column_3_index];
			board[2][2] = get_next_slot(column_3, COLUMN_SIZE, column_3_index);

			board[2][0]->position = { SLOT_SIZE * 2, 0 };
			board[2][1]->position = { SLOT_SIZE * 2, SLOT_SIZE };
			board[2][2]->position = { SLOT_SIZE * 2, SLOT_SIZE * 2 };

			if (board[0][1] == board[1][1]) {
				score += board[0][1]->points;

				if (board[0][1] == board[2][1]) {
					score += board[0][1]->points;
				}
			}

			if (board[0][0] == board[1][1]) {
				score += board[0][0]->points;

				if (board[0][0] == board[2][2]) {
					score += board[0][0]->points;
				}
			}

			if (board[0][2] == board[1][1]) {
				score += board[2][0]->points;

				if (board[2][0] == board[0][2]) {
					score += board[2][0]->points;
				}
			}

			play_timer = 1.0f;
		}
        
        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                draw_slot(board[i][j], renderer);
            }
        }
		
		SDL_RenderPresent(renderer);
    }
    
	return 0;
}
