#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
extern char *shared_memory;
#define LINE_0 (shared_memory + 65*21)
#define LINE_1 (shared_memory + 65*5)
#define LINE_BASE (shared_memory)
#define LINE_NOPE (shared_memory+65*12)

#endif
