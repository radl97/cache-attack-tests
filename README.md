# Egyszerű cache támadás

Itt található egy egyszerű cache támadásra példa. Flush+Reload a technika neve.

Egyelőre **Linux-specifikus** kódokat tartalmaz a lehető legjobb működés elérése érdekében.

A példa támadásnál 4 mintát mérünk, ebből első kettőnél megnézzük a memória tartalmát közvetlen a mérés előtt, az utolsó kettőnél nem.

40-szer mérést végez, és mind a 4x40 mérés eredményét kiírja.

## A Flush+Reload lényege

Azt szeretnénk megnézni ennél a támadásnál, hogy konkrét memóriaterületet elért-e a támadó egy bizonyos időablakban.

1) Először kiűrítjük a cache-ből az adott területet,
2) Várunk (ezen az időablakon belül tudjuk észlelni majd a memóriaelérést)
3) Megmérjük, mennyi idő alatt érjük el az előbb kiűrített memóriaterületet.
4) Ha ez a szám "kicsi" (100-200 órajel), akkor a cache-ből értük el, tehát egy másik program hozzáfért. Egyébként (500+ órajel) nem értük el.

## Parancssoros tesztelés

Fordítás:
`g++ spy.cpp -o run -O3`

Futtatás:
`./run`

## Eredmények

Nekem az eredményeim:
```
149 137 8842 8878
145 134 497 509
130 137 497 497
137 137 803 488
133 146 464 465
141 145 500 565
138 153 513 501
137 142 501 488
162 138 469 508
137 154 548 541
161 162 468 537
149 145 516 1017
146 149 505 508
142 145 501 497
137 145 525 536
158 138 529 492
137 145 505 521
154 149 501 532
145 141 512 537
158 153 512 517
137 145 908 492
153 134 549 493
154 153 505 541
154 149 520 500
161 153 524 521
157 157 517 513
145 137 512 504
145 150 2474 521
141 154 516 525
137 166 508 501
141 145 1175 525
150 141 501 504
142 145 1235 504
146 145 505 508
138 149 517 525
170 141 505 553
145 145 509 509
141 141 512 509
169 146 492 505
153 157 516 520
```

Látszik, hogy az első két mérés esetén látványosan kevesebb idő telik el,
ami annyit jelent, hogy a cache-ből kellett csak előhozni az adatokat, nem
kellett a memóriának (vagy lassabb cache-nek) szólni.
