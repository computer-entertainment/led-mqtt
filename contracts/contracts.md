
# Datentypen
Damit weniger topics an den NodeMCUs verwendet werden müssen, werden
einige Informationen gebündelt als `struct` versendet. Hier wird die
Bedeutung der einzelnen Parameter beschrieben.

Falls nicht anders angegeben, sind alle Zahlen *little-endian*.

## `struct` ledState
Dieser Datentyp beschreibt einen Zustand der LED, bestehend aus einer
Animation und Parametern für diese Animation.
- `animationId` (`uint8`)  
  Die Kennung der aktuellen Animation.
- `brightness` (`uint8`)  
  Helligkeit der LEDs. Bei einem Wert von `0` sind alle LEDs
  ausgeschaltet, bei `255` liegt die volle Helligkeit vor.
- `animationSize` (`uint16`)  
  Größe bzw. Dauer der Animation.
- `speed` (`uint16`)  
  Geschwindigkeit der Animation. Größere Werte führen zu dazu, dass
  die Animation **langsamer** wird.
- `color` (`uint32`, bzw. `-RGB`)  
  Die Hauptfarbe der Animation. Wird nicht von jeder Animation
  berücksichtigt.
- `colorRotation` (`float`)  
  Zeitliche Winkeländerung der Farbe.
- `decay` (`uint8`)  
  Bei Werten ungleich `0` bleibt jeweils das vorherige Frame in
  gedimmt erhalten. Es entsteht der Effekt, dass alte Frames mit der
  Zeit verblassen.
- `fadeId` (`uint8`)  
  Kennung der Überblendung, die beim Wechsel von Animationen verwendet
  werden soll.
- `fadeSpeed` (`uint16`)  
  Dauer einer Überblendung, größere Werte bedeuten eine längere
  Überblendungszeit.
- `colorRotationEffect` (`uint8`)  
  Gibt an wieviel von der Ursprünglichen Farbe, die durch `color` gesetzt wird, erhalten bleibt.
  Bei einem Wert von `0` wird `colorRotation` ignoriert, bei einem Wert von `255` wird `color` ignoriert.

# Kennzahlen
Die Animationen und Überblendungen werden von Kennzahlen beschrieben.

## Animationen
- `0`: Ausgeschaltet
- `1`: Solide Farbe
  - Die Farbe wird durch `color` festgelegt
- `2`: Regenbogen
- `3`: Knight Rider 
- `4`: Strope 

## Überblendungen
- 
