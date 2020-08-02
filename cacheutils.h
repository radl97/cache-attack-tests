#ifndef CACHEUTILS_H
#define CACHEUTILS_H

// Maradjon C-kód
#ifdef __cplusplus
extern "C" {
#endif

/**
 Hasznos függvények közvetlen, a mi célunkhoz megfelelően optimalizált
memóriahozzáférések elvégzéséhez.
*/

#include <stdint.h> // bitméret alapján számtípusok: pl. uint32_t

// Force inline:
// A függvényhívás költségeitől megtérülünk.
// Képes javítani az mérés eredményeit.
// Gyakorlatban over-kill.
#define INLINE __attribute__((always_inline)) static inline

/**
Mérést végez annak kiderítésére, hogy ennek a függvénynek
az előző futása óta más fért-e hozzá a memóriaterülethez.

Használat: 

probe_timing(&(buffer[i]));
// Az "áldozat" kód hozzáfér a kódhoz... vagy nem.
measurement = probe_timing(&(buffer[i]));

// a 300 nálam kimért érték, más gépnél más érték előfordulhat!
if (measurement < 300) {
	printf("Az áldozat kód valószínűleg hozzáfért a megadott memóriaterülethez!");
}

Flush+reload alapon olvasási idő mérése.
Meglehetősen optimalizált verzió.

Ehhez hasonló működéssel bír:

int probe_timing(void* ptr) {
	int t1 = get_time();
    *ptr; // itt hozzáférünk a memóriaterülethez
	int t2 = get_time();
	remove_from_cache(*ptr);
	return t2-t1; // mennyi időt vett igénybe, míg elértük az értéket?
}

Sok varázslatot használ, ami nekünk innentől nem fog kelleni,
de érdekesség szintjén megpróbálom leírni.

- INLINE kulcsszó: fentebb definiáltuk.
  a függvényhívás ebben a helyzetben drágának tekinthető, érdemes tőle megszabadulni.

  Nagyon speciális esetben érdemes használni (például itt).

  Túlzott használat esetén nem fordul le.

- "uint32_t" egy 32 bites előjel nélküli szám.

  Ebben fogjuk tárolni az igénybe vett időt.

- "register" kulcsszó: a változó értékét NE tárold memóriában,
  csak regiszterben. Ezáltal pár memóriahozzáféréstől megszabadulhatunk.

  A paraméternél a pointerre vonatkozik, nem a pointer által mutatott területre.

  Nagyon speciális esetben érdemes használni (például itt).

  Túlzott használat esetén nem fordul le (nincs elég regiszter).

- "asm": Egy assembly kód következik. Architektúra specifikus,
  érdemes távolról kerülni.

  Az "asm volatile" továbbá azt mondja meg, hogy a fordító ne próbáljon meg okoskodni.
  
  Itt azért kell, mert nagyon pontosan kell elmagyaráznunk a gépnek,
  hogy mit akarunk, és nagyon gyors megoldást akarunk.

  Látványosan javítja a mérési eredményeket.

Az assembly utasítások rövid magyarázata:
  "mfence" és "lfence" kulcsszavakat egyelőre mellőzzük.
  A lényeges mozzanat, hogy megvárja, amíg megtörténnek bizonyos memóriaműveletek,
  mielőtt bizonyos más memóriaműveletekre sort ejtene.

rdtsc: Mérd meg az időt.
  Az rdtsc időmérés helyett pontosabban az elvégzett utasítások számára ad egy becslést,
  tehát például függhet a processzor sebességétől.

(*)-gal jelölt sor: Érd el a pointer által mutatott pointert (char*-ként értelmezve).

A többi művelet kevésbé érdekes számunkra, regiszter másolás (mov) illetve egyszerű kivonás (sub).

A C-be ágyazott ASM kód elég bonyolult szintaxissal rendelkezik, hogy
a lehető legtöbbet tudjon megadni a programozó, de az optimalizálás is hatásos legyen az assembly kód körül.
- a %0 és %1 jelöli a time illetve az adrs C változót, amiket lentebb ad meg.
*/
INLINE uint32_t probe_timing(register void *adrs) {
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

/**
"Érd el" a megadott memóriaterületen lévő adatot.
Memory probe/memory access néven szokták említeni angolul.
*/
INLINE void maccess(register void* p)
{
  asm volatile ("movb (%0), %%al\n"
    :
    : "c" (p)
    : "rax");
}

#ifdef __cplusplus
} // extern C
#endif

#endif
