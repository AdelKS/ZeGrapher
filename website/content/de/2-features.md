# Funktionen

ZeGrapher zeichnet mathematische Objekte in der Ebene — Funktionen, Folgen,
Parameterdarstellungen und Daten — und gibt genau das aus, was du siehst.

- **Funktionen** schreibst du wie auf Papier, `f(x) = 2 + cos(x)`. Die üblichen
  mathematischen Funktionen sind da, und jedes Objekt, das du selbst festlegst.
- **Folgen** aus ihrer Rekursion, die ersten Glieder gleich mit:
  `u(n) = 0 ; 1 ; u(n-2) + u(n-1)`.
- **Parameterdarstellungen**, gezeichnet über den Bereich, den du wählst.
- **Konstanten und Variablen**, die allen Objekten gemeinsam sind: ein einziger
  Wert steuert dann eine ganze Kurvenschar. Eine Konstante kann:
    - am Regler zwischen zwei Grenzen wandern, von Hand oder als Animation
    - mehrere Werte auf einmal annehmen, und jede Kurve, die sie benutzt, wird
      einmal je Wert gezeichnet
- **Daten** aus CSV, und in einer Tabelle zu bearbeiten wie in einer
  Tabellenkalkulation. Erprobt an Dateien mit mehreren Millionen Zellen.
- **Objekte, die aufeinander aufbauen**: jedes Objekt mit Namen darf im Ausdruck
  eines anderen Objekts stehen, und in den Zellen einer Datenspalte.
- **Ein Graph, den du bewegst**: ziehen verschiebt, scrollen zoomt, und jede
  Achse lässt sich einzeln zoomen.
- **Einstellbar bis aufs Gitter**: Teilstriche als Vielfache eines Ausdrucks wie
  `π`, und eigene Teilungen und Farben für x und für y.
- **Ausgaben, die der Vorschau genau entsprechen**, als Vektor (`svg`, `pdf`)
  oder als Bild (`png`, `jpeg`, `bmp`, `ppm`). Die Größe gibst du in echten
  Zentimetern oder in Pixeln an.
- **Mach da weiter, wo du aufgehört hast**: das Programm speichert deine Arbeit,
  wenn du es schließt, und holt sie beim nächsten Start zurück. Du kannst sie
  auch in einer eigenen Datei speichern und jederzeit wieder öffnen.
