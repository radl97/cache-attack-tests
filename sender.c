#include "sharedlib.h" // char* shared_memory
#include "cacheutils.h" // maccess(void* ptr), int probe_timing(void* ptr)
#include <sched.h> // sched_yield()
#include <unistd.h> // usleep(int)

uint64_t sum = 0;

INLINE void send(int bit) { // no "bool" in C
	if (bit)
		sum+=maccess(LINE_0);
	else
		sum+=maccess(LINE_1);
}

int main() {
	while (1) { // true and false are not present in C
		printf("3\n");
		for (long long i = 0; i < 10000000000ll; i++) {
			sum += maccess(LINE_BASE);
			send(0);
			send(1);
		}
		printf("2\n");
		for (long long i = 0; i < 20000000000ll; i++) {
			sum += maccess(LINE_BASE);
			send(1);
		}
		printf("1\n");
		for (long long i = 0; i < 20000000000ll; i++) {
			sum += maccess(LINE_BASE);
			send(0);
		}
		printf("0\n");
		for (long long i = 0; i < 30000000000ll; i++) {
			sum += maccess(LINE_BASE);
		}
		printf("%d\n", sum);
	}
}
