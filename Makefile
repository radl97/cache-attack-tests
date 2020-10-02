# These are not file-targets
.PHONY: runsender runreceiver

# runs sender pinned to the first CPU core
runsender: sender
	LD_LIBRARY_PATH=. taskset -c 1 ./sender

# runs receiver pinned to the first CPU core
runreceiver: receiver
	LD_LIBRARY_PATH=. taskset -c 0 ./receiver

sender: libsharedlib.so sender.c cacheutils.h sharedlib.h
	gcc sender.c -Wall -Wextra -o sender -O3 -lsharedlib -L.

receiver: libsharedlib.so receiver.c cacheutils.h sharedlib.h
	gcc receiver.c -Wall -Wextra -o receiver -O3 -lsharedlib -L.

libsharedlib.so: sharedlib.c sharedlib.h
	gcc -fpic sharedlib.c -Wall -Wextra -shared -o libsharedlib.so -O3

