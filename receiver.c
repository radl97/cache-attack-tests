#include "sharedlib.h" // char* shared_memory
#include "cacheutils.h" // maccess(void* ptr), int probe_timing(void* ptr)
#include <sched.h> // sched_yield()
#include <unistd.h> // usleep(int)

int main() {
	// TODO
	while (1) {
		sched_yield();

		register long long a = probe_timing(shared_memory+0);
		//sched_yield();
		//register long long b = probe_timing(shared_memory+1);

		for (int i = 0; i < a;  i+=50)
			printf(" ");
		printf("%lld\n", a);
		usleep(10000);
	}
}
