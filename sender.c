#include "sharedlib.h" // char* shared_memory
#include "cacheutils.h" // maccess(void* ptr), int probe_timing(void* ptr)
#include <sched.h> // sched_yield()
#include <unistd.h> // usleep(int)

void send(int bit) { // no "bool" in C
	// TODO
}

int main() {
	while (1) { // true and false are not present in C
		send(0);
		send(1);
	}
}
