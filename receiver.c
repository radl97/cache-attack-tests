#include "sharedlib.h" // char* shared_memory
#include "cacheutils.h" // maccess(void* ptr), int probe_timing(void* ptr)
#include <sched.h> // sched_yield()
#include <unistd.h> // usleep(int)

#define PROOF_LIMIT 500 // what number of cycles constitute as a proof of an unloaded cacheline
#define ITERATION_LENGTH 300 // how many samples are aggregated for a decision

// number of proofs needed to say that a cacheline was not used
// corresponds to bit 1 (value 0 or 2)
#define NUM_PROOF_RED 7
// number of proofs needed to say that a cacheline was not used
// corresponds to bit 0 (value 0 or 1)
#define NUM_PROOF_GREEN 7

// configuring according to channel noise:
// 1->3 and 0->2 : try lowering NUM_PROOF_RED
// 2->3 and 0->1 : try lowering NUM_PROOF_GREEN

int main() {
	int ctr = 0;
	int red_proofs_ctr = 0;
	int green_proofs_ctr = 0;
	while (1) {

		register long long ref = probe_timing(LINE_NOPE);
		register long long red = probe_timing(LINE_0);
		register long long green = probe_timing(LINE_1);
		//sched_yield();
		//register long long b = probe_timing(shared_memory+1);
		if (green > PROOF_LIMIT) {
			green_proofs_ctr++; // PROOF that green is 0
		}
		if (red > PROOF_LIMIT) {
			red_proofs_ctr++;	// PROOF that red is 0
		}
		ctr++;
		if (ctr == ITERATION_LENGTH) {
			printf("\x1B[31mBLIP%lld\x1B[0m\n", red_proofs_ctr);
			printf("\x1B[32mBLOOP%lld\x1B[0m\n", green_proofs_ctr);
			printf("\x1B[33m%lld\x1B[0m\n", red);
			printf("\x1B[33m%lld\x1B[0m\n", green);
			printf("\x1B[33m%lld\x1B[0m\n", ref);

			int guess = 0;
			if (red_proofs_ctr < NUM_PROOF_RED) { guess |= 2; }
			if (green_proofs_ctr < NUM_PROOF_GREEN) { guess |= 1; }
			printf("\x1B[1m%lld\x1B[0m", guess);

			ctr = 0;
			red_proofs_ctr = 0;
			green_proofs_ctr = 0;
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
