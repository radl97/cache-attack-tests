char shared_buffer[4096] __attribute__((aligned(4096)));
char* const shared_memory __attribute__((visibility("default"))) = shared_buffer;
