// Flush+reload optimalizálása

int probe_timing_v0(char* ptr) {
	remove_from_cache(*ptr);
	// Várunk...
	int t1 = get_time();
    *ptr; // itt hozzáférünk a memóriaterülethez
	int t2 = get_time();
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

// Gond: a fordító kioptimizálja a memóriahozzáférést
// A megoldás a volatile kulcsszó, ami azt jelenti, hogy a memóriahozzáférést nem helyezheti 
// a *fordító* arrébb*

int probe_timing_v1(void* ptr) {
	remove_from_cache(*ptr);
	// Várunk...
	int t1 = get_time();
    (volatile char*)*ptr; // itt hozzáférünk a memóriaterülethez
	int t2 = get_time();
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

int probe_timing_v2(void* ptr) {
	remove_from_cache(*ptr);
	// Várunk...
	int t1 = get_time();
    (volatile char*)*ptr; // itt hozzáférünk a memóriaterülethez
	int t2 = get_time();
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

// Ha csak az előző alkalom óta hozzáfért memóriaterület érdekel, akkor a
// "Várás" lehet a "minden más, amit csinálunk"
// Ha mértünk, akkor rögtön kivesszük a memóriaterületet

int probe_timing_v3(void* ptr) {
	int t1 = get_time();
    (volatile char*)*ptr; // itt hozzáférünk a memóriaterülethez
	int t2 = get_time();
	remove_from_cache(*ptr);
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

// Gond: a műveletek (mérési pontok a memóriahozzáférés elé/mögé kerülhet) sorrend cserélődhet a fordító miatt
// Megoldás: Az asm volatile (""); varázsige ehhez a legjobb
// Gond: a műveletek (mérési pontok a memóriahozzáférés elé/mögé kerülhet) sorrend cserélődhet a processzor működése miatt (out-of-order execution)
// Megoldás: Van olyan utasítás (fence), ami bizonyos utasítások sorrendjét nem engedi felcserélni
//   Ezeket két különböző processzoron lévő szinkronizálására használják
//   Jól dokumentált (nem x86-specifikus): https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/memory-barriers.txt

void generic_barrier() {
	asm volatile ("mfence");
	asm volatile ("lfence");
}

int probe_timing_v3(void* ptr) {
	int t1 = get_time();
	generic_barrier();
    (volatile char*)*ptr; // itt hozzáférünk a memóriaterülethez
	generic_barrier();
	int t2 = get_time();
	generic_barrier();
	remove_from_cache(*ptr);
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

// Gond: Függvényhívás elképesztően drága ott, ahol a generic_barrier()-t használni akarjuk
//   Igazából csak belemásolnánk a kódba azt az egy sort a hívás helyére...
// Megoldás: force-inlining
//   static: csak ebben a C fájlban jelenik majd meg a függvény, amit írunk
//   inline: érdemes megpróbálni a függvényhívást "inline"-olni, azaz a hívás helyére illeszteni a függvény belsejét.
//   
// (Másik megoldás lenne a #define, de ha van rá más eszköz, akkor ne alkalmazzunk makrót)

#define INLINE static inline __attribute__((always_inline))

INLINE void generic_barrier_v2() {
	asm volatile ("mfence");
	asm volatile ("lfence");
}

// Gond: még így is lehetnek nem várt dolgok, a legjobb, ha assembly-ben írjuk :3

INLINE uint32_t long probe_timing_v4(void *adrs) {
    volatile uint32_t time;
    // TODO nem mindegyikhez kell megadni regisztert.
    asm volatile (
        "    mfence             \n"
        "    lfence             \n"
        "    rdtsc              \n"
        "    lfence             \n"
        "    movl %%eax, %%esi  \n"
        "    movb (%1), %%al    \n" // (*)
        "    lfence             \n"
        "    rdtsc              \n"
        "    mfence             \n"
        "    subl %%esi, %%eax  \n"
        "    clflush 0(%1)      \n"
        "    mfence             \n"
        : "=a" (time)
        : "c" ((char*)adrs)
        : "%esi", "%edx"
    );
    return time;
}

// Gond: adott esetben nem tenné regiszterbe a változóinkat (adrs, time),
// és még további műveleteket illesztene be a fordító, hogy beolvassa őket memóriából
// Megoldás: jelöljük meg "register" kulcsszóval, ami azt jelenti, hogy mindenképp
// regiszterben kell tárolni az értékét.
// Így biztosak lehetünk abban, hogy a kód csak akkor fordul a memóriához (adatért),
// amiket szeretnénk. (természetesen az instructionökért nem felelhetünk)

INLINE uint32_t long probe_timing_v4(register void *adrs) {
    register volatile uint32_t time;
    // TODO nem mindegyikhez kell megadni regisztert.
    asm volatile (
        "    mfence             \n"
        "    lfence             \n"
        "    rdtsc              \n"
        "    lfence             \n"
        "    movl %%eax, %%esi  \n"
        "    movb (%1), %%al    \n" // (*)
        "    lfence             \n"
        "    rdtsc              \n"
        "    mfence             \n"
        "    subl %%esi, %%eax  \n"
        "    clflush 0(%1)      \n"
        "    mfence             \n"
        : "=a" (time)
        : "c" ((char*)adrs)
        : "%esi", "%edx"
    );
    return time;
}

