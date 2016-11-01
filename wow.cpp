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

#define SLOT_SIZE 10

int main() {
	printf("Slot machine started\n");
    srand(time(NULL));
    
	const char* values[] = {
		"7",
		"Cherry",
		"Lemon",
		"Orange",
		"Bar"
	};
	
	const char* slot1[SLOT_SIZE];
	const char* slot2[SLOT_SIZE];
	const char* slot3[SLOT_SIZE];
	
	for (u32 i = 0; i < SLOT_SIZE; i++) {
		slot1[i] = values[rand() % 5];
		slot2[i] = values[rand() % 5];
		slot3[i] = values[rand() % 5];
	}
	
	u32 score = 0;
	
	bool is_running = true;
	while (is_running) {
		
		const char* result1 = slot1[rand() % SLOT_SIZE];
		const char* result2 = slot2[rand() % SLOT_SIZE];
		const char* result3 = slot3[rand() % SLOT_SIZE];
		
		printf("%s\t\t%s\t\t%s\n", result1, result2, result3);
		
		if (result1 == result2 && result1 == result3) {
			score += 100;
			printf("You won that time! Score: %u\n", score);
		}
		
		sleep(1);
	}
	
	printf("Slot machine ended\n");
}