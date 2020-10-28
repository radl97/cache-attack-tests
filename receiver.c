#include "sharedlib.h" // char* shared_memory
#include "cacheutils.h" // maccess(void* ptr), int probe_timing(void* ptr)
#include <sched.h> // sched_yield()
#include <unistd.h> // usleep(int)

int main() {
	int ctr = 0;
	int RED = 0;
	int GREEN = 0;
	while (1) {

		register long long ref = probe_timing(LINE_NOPE);
		register long long red = probe_timing(LINE_0);
		register long long green = probe_timing(LINE_1);
		//sched_yield();
		//register long long b = probe_timing(shared_memory+1);
		if (green > 500) {
			GREEN++; // PROOF that green is 0
		}
		if (red > 500) {
			RED++;	// PROOF that red is 0
		}
		ctr++;
		if (ctr == 300) {
			printf("\x1B[31mBLIP%lld\x1B[0m\n", RED);
			printf("\x1B[32mBLOOP%lld\x1B[0m\n", GREEN);
			printf("\x1B[33m%lld\x1B[0m\n", red);
			printf("\x1B[33m%lld\x1B[0m\n", green);
			printf("\x1B[33m%lld\x1B[0m\n", ref);

			int guess = 0;
			if (RED < 10) { guess |= 2; }
			if (GREEN < 10) { guess |= 1; }
			printf("\x1B[1m%lld\x1B[0m", guess);

			ctr = 0;
			RED = 0;
			GREEN = 0;
		}

/*
		for (int i = 0; i < a;  i+=50)
			printf(" ");
		printf("\x1B[31m%lld\x1B[0m\n", a);
		for (int i = 0; i < b;  i+=50)
			printf(" ");
		printf("\x1B[32m%lld\x1B[0m\n", b);
		for (int i = 0; i < ref;  i+=50)
			printf(" ");
		printf("\x1B[33m%lld\x1B[0m\n", ref);*/
		usleep(100);
	}
}
