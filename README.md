# Mustersuche

Implementierung des `Shift_And`, `Horspool` und des `BNDM` Algorithmus in C++.
Zum messen der Laufzeiten wurde [google/benchmark](https://github.com/google/benchmark) verwenden (wird zum compilieren benötigt).

## Laufzeiten
Getestet wurde immer mit einer Textgröße von 8Mio bytes.
Auf der x-Achse ist die Mustergröße von 2 bis 64 bytes.
Auf der y-Achse die durchschnittliche Laufzeit des Algorithmus in millisekunden.

Laufzeit mit großem Alphabet (Werte [1, 255]):
![großes Alphabet](https://github.com/Bricktricker/Mustersuche/blob/master/img/plot_full.jpg?raw=true)

Laufzeit mit mittelgroßem Alphabet (Werte [33, 126]):
![mittleres Alphabet](https://github.com/Bricktricker/Mustersuche/blob/master/img/plot_medium.jpg?raw=true)

Laufzeit mit kleinem Alphabet (Werte [1, 4])
![kleines Alphabet](https://github.com/Bricktricker/Mustersuche/blob/master/img/plot_small.jpg?raw=true)
