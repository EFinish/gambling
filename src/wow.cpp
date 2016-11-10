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

#define PIXELS_PER_UNIT 32.0f

struct v2 {
    r32 x;
    r32 y;
};

struct Slot {
    SDL_Texture* texture;
    v2 position;
    v2 size;
    u32 points;
};

void draw_slot(Slot* slot, SDL_Renderer* renderer) {
    SDL_Rect rect = { 
        (i32)(slot->position.x * PIXELS_PER_UNIT), 
        (i32)(slot->position.y * PIXELS_PER_UNIT), 
        (i32)(slot->size.x * PIXELS_PER_UNIT), 
        (i32)(slot->size.y * PIXELS_PER_UNIT) 
    };

    SDL_RenderCopy(renderer, slot->texture, NULL, &rect);
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

	SDL_Surface* bell = SDL_LoadBMP("../assets/bell.bmp");
	SDL_Texture* bellTexture = SDL_CreateTextureFromSurface(renderer, bell);
	SDL_FreeSurface(bell);

    SDL_Surface* cherry = SDL_LoadBMP("../assets/cherry.bmp");
	SDL_Texture* cherryTexture = SDL_CreateTextureFromSurface(renderer, cherry);
	SDL_FreeSurface(cherry);

    SDL_Surface* jewel = SDL_LoadBMP("../assets/jewel.bmp");
	SDL_Texture* jewelTexture = SDL_CreateTextureFromSurface(renderer, jewel);
	SDL_FreeSurface(jewel);

    SDL_Surface* orange = SDL_LoadBMP("../assets/orange.bmp");
	SDL_Texture* orangeTexture = SDL_CreateTextureFromSurface(renderer, orange);
	SDL_FreeSurface(orange);

    SDL_Surface* seven = SDL_LoadBMP("../assets/seven.bmp");
	SDL_Texture* sevenTexture = SDL_CreateTextureFromSurface(renderer, seven);
	SDL_FreeSurface(seven);

    SDL_Surface* singleBar = SDL_LoadBMP("../assets/singleBar.bmp");
	SDL_Texture* singleBarTexture = SDL_CreateTextureFromSurface(renderer, singleBar);
	SDL_FreeSurface(singleBar);

    SDL_Surface* tripleBar = SDL_LoadBMP("../assets/tripleBar.bmp");
	SDL_Texture* tripleBarTexture = SDL_CreateTextureFromSurface(renderer, tripleBar);
	SDL_FreeSurface(tripleBar);

    printf("Slot machine started\n");
    srand(time(NULL));
        
    Slot slots[] = {
        { bellTexture, 100, 100, 35},
        { cherryTexture, 100, 100, 20},
        { jewelTexture, 100, 100, 85},
        { orangeTexture, 100, 100, 50},
        { sevenTexture, 100, 100, 100},
        { singleBarTexture, 100, 100, 0},
        { tripleBarTexture, 100, 100, 10}
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
	i32 x = 0;
    
    u64 timer_frequency = SDL_GetPerformanceFrequency();
    u64 last_time = SDL_GetPerformanceCounter();

    v2 position = {};

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

        u32 column_1_index = rand() % COLUMN_SIZE;
        u32 column_2_index = rand() % COLUMN_SIZE;
        u32 column_3_index = rand() % COLUMN_SIZE;
        
        Slot* board[3][3];
        
        board[0][0] = get_previous_slot(column_1, COLUMN_SIZE, column_1_index);
        board[0][1] = column_1[column_1_index];
        board[0][2] = get_next_slot(column_1, COLUMN_SIZE, column_1_index);
        
        board[1][0] = get_previous_slot(column_2, COLUMN_SIZE, column_2_index);
        board[1][1] = column_2[column_2_index];
        board[1][2] = get_next_slot(column_2, COLUMN_SIZE, column_2_index);
        
        board[2][0] = get_previous_slot(column_3, COLUMN_SIZE, column_3_index);
        board[2][1] = column_3[column_3_index];
        board[2][2] = get_next_slot(column_3, COLUMN_SIZE, column_3_index);

        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                printf("%d",board[i][j]->points);
            }
        }
            
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

        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                draw_slot(board[i][j], renderer);
            }
        }

		SDL_RenderPresent(renderer);
    }
    
	return 0;
}
