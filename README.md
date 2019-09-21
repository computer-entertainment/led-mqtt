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
Sofern nicht anders angegeben sind alle Zahlen *little-endian*. In
einigen Topics werden Daten direkt in Form von `structs` verschickt,
deren Form in [contracts](contracts) festgelegt sind.

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
  - `ledState` (`struct ledState`)  
    Legt die angezeigte Animation und alle Standardparameter von
    Animationen fest. Diese sind in
    [contracts/basetypes.h](contracts/basetypes.h) dokumentiert.
- `remote`  
  Anfragen, die von der Fernbedienung stammen. Diese werden von der
  Fernbedienung festgelegt und von dem Controller nach `display`
  übertragen.
  - `ledState` (`struct ledState`)  
    Legt die anzuzeigende Animation und alle Standardparameter von
    Animationen fest. Diese sind in
    [contracts/basetypes.h](contracts/basetypes.h) dokumentiert.
- `music`  
  Anfragen, die von der Audiosteuerung stammen.
  
  Dieses Modul unterscheidet sich in der Steuerung ein wenig von den
  normalen Modulen, da eine möglichst geringe Latenz vorliegen
  soll. Dazu wird die Musik auf dem Raspberry PI, der den Controller
  preview  ausführt, analysiert und die resultierenden Anfragen direkt in
  `display` geschrieben. So müssen die Werte nicht mehr in dieses
  Topic kopiert werden.
