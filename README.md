# LED-Zentralsteuerung

## Programmteile
- Steuerung von LED-Streifen  
  Die Steuerung des LED-Streifens wird von einem NodeMCU übernommen,
  der per MQTT auf Anfragen für Animationen reagiert.
  
  Die Anfragen bestehen dabei nur aus einer Animations-ID und
  möglicherweise zusätzlichen Parametern (s.u.). Die Berechnung der
  resultierenden Helligkeitswerte zu bestimmten Zeitpunkten wird von
  dem NodeMCU durchgeführt.
- MQTT-Fernbedienung  
  Die MQTT-Fernbedienung ist eine mögliche Quelle für
  Animationsanfragen. Mit der Fernbedienung kann eine Animation
  ausgewählt werden und ein zusätzlicher Parameter angepasst werden.
  
  Außerdem kann angegeben werden, dass die Fernbedienung keine Anfrage
  liefern soll, damit in diesem Fall auf andere Quellen reagiert
  werden kann.
- MQTT-Broker  
  Der verwendete MQTT-Broker kann grundsätzlich beliebig gewählt
  werden, sollte aber eine möglichst geringe Latenz zu den restlichen
  Komponenten besitzen.
- Controller  
  Der Controller entscheidet aus den Anfragen von verschiedenen
  Quellen, welche Animation und welche zugehörigen Parameter
  tatsächlich angezeigt werden sollen.

## Steuermodule
Im Folgenden werden kurz die Module beschrieben, die Anfragen für die
LEDs machen können. Dabei werden die Steuermodule nach absteigender
Priorität sortiert angegeben:

- `remote`  
  Die MQTT-Fernbedienung.
- `music`  
  Die Audiosteuerung.
- `idle`  
  Kein reguläres Modul ist aktiv, es gibt also keine konkreten
  Anfragen. In diesem Fall darf die Steuerung entscheiden, welche
  Animationen angezeigt werden, im Normalfall werden alle LEDs
  ausgeschaltet.


## Topics
Sofern nicht anders angegeben sind alle Zahlen *little-endian*.

- `control`  
  Informationen zu dem Zustand des Controllers.
  - `activeModule` (Text)  
    Gibt an, welches Modul aktuell die Steuerung der LEDs
    übernimmt. Dabei kommen möglicherweise mehrere Module infrage und
    das von Controller priorisierte Modul wird hier angegeben.
  - `uptime` (4 Byte)  
    Gibt in Sekunden an, wie lange der Controller bereits aktiv ist.
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
