#include "cacheutils.h"
#include <stdio.h>
#include <sched.h>

struct cache_line {
	char _[4096];
} __attribute__((aligned(4096)));

struct cache_line buffer[4];

#define INLINE __attribute__((always_inline)) static inline

int main() {
	void *ptr = nullptr;

	for (int j = 0; j < 40; j++) {

		sched_yield();
		maccess(buffer+0);
		register long long a0 = probe_timing(buffer+0);

		sched_yield();
		maccess(buffer+1);
		register long long a1 = probe_timing(buffer+1);

		sched_yield();
		register long long a2 = probe_timing(buffer+2);

		sched_yield();
		register long long a3 = probe_timing(buffer+3);

		printf("%lld %lld %lld %lld\n", a0, a1, a2, a3);
	}
}
