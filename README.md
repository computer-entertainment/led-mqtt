# LED-Zentralsteuerung


## Topics
Sofern nicht anders angegeben sind alle Zahlen *little-endian*.

- `display`  
  Enthält alle Informationen zu dem Inhalt, der aktuell angezeigt
  werden soll. Diese Informationen werden nicht direkt von den
  Anwendungen verändert, sondern von dem Controller festgelegt, der
  die Prioritäten für die Anwendungen setzt.
  - `animation` (1 byte)  
    Die ID der aktuellen Animation. Die verwendeten
    Animationsparameter hängen davon ab, welche Animation ausgewählt
    wird.
  - `color` (3 bytes, RGB)  
    Die Hauptfarbe der LEDs, falls von der Animation unterstützt.
  - `brightness` (1 byte)  
	Gibt die Gesamthelligkeit der LEDs an. Bei `0` sind alle LEDs aus,
    bei `255` liegt die volle Helligkeit vor.
  - `speed` (2 bytes)  
	Legt die Geschwindigkeit der Animation fest. Dabei ist die
	Animation bei einem __größeren__ Wert __langsamer__.
- `fernbedienung`  
  Anfragen, die von der Fernbedienung stammen. Diese werden von der
  Fernbedienung festgelegt und von dem Controller nach `display`
  übertragen.
  - *folgt noch*
