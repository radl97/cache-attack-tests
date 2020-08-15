# These are not file-targets
.PHONY: runall runsender runreceiver

# runs both sender and receiver at once
runall:
	LD_LIBRARY_PATH=. taskset -c 0 ./sender &\
LD_LIBRARY_PATH=. taskset -c 0 ./receiver

# runs sender pinned to the first CPU core
runsender: sender
	LD_LIBRARY_PATH=. taskset -c 0 ./sender

# runs receiver pinned to the first CPU core
runreceiver: receiver
	LD_LIBRARY_PATH=. taskset -c 0 ./receiver

sender: libsharedlib.so sender.c cacheutils.h sharedlib.h
	gcc sender.c -o sender -O3 -lsharedlib -L.

receiver: libsharedlib.so receiver.c cacheutils.h sharedlib.h
	gcc receiver.c -o receiver -O3 -lsharedlib -L.

libsharedlib.so: sharedlib.c sharedlib.h
	gcc -fpic sharedlib.c -shared -o libsharedlib.so -O3

