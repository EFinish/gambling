#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
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

struct Slot {
	const char* name;
	u32 points;
};

Slot* get_previous_slot(Slot** slots, u32 slots_count, u32 slot_index) {
	assert(slot_index >= 0 && slot_index < slots_count);

	Slot* result = NULL;
	
	if (slot_index - 1 < 0) {
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

i32 main() {
	printf("Slot machine started\n");
    srand(time(NULL));
		
	Slot slots[] = {
		{ "7", 100 },
		{ "Cherry", 25 },
		{ "Lemon", 75 },
		{ "Orange", 50 },
		{ "Bar", 10 }
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
	
	bool is_running = true;
	while (is_running) {
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
		
		printf("%s\t\t%s\t\t%s\n%s\t\t%s\t\t%s\n%s\t\t%s\t\t%s\n\n", 
			board[0][0]->name, 
			board[1][0]->name, 
			board[2][0]->name,
			board[0][1]->name, 
			board[1][1]->name, 
			board[2][1]->name,
			board[0][2]->name, 
			board[1][2]->name, 
			board[2][2]->name);
			
		if (board[0][1] == board[1][1]) {
			score += board[0][1]->points;

			if (board[0][1] == board[2][1]) {
				score += board[0][1]->points;
			}

			printf("Horizontal, new score: %u\n", score);
		}
		
		if (board[0][0] == board[1][1]) {
			score += board[0][0]->points;
			
			if (board[0][0] == board[2][2]) {
				score += board[0][0]->points;
			}

			printf("Left diagonal, new score: %u\n", score);
		}

		if (board[0][2] == board[1][1]) {
			score += board[2][0]->points;
			
			if (board[2][0] == board[0][2]) {
				score += board[2][0]->points;
			}

			printf("Right diagonal, new score: %u\n", score);
		}

		sleep(1);
	}
	
	printf("Slot machine ended\n");
}