# PresepeV2

Base Arduino Uno / PlatformIO per uno spettacolo attivato da un sensore a ultrasuoni:
stella cometa con servo, musica natalizia e luci animate.

## Comportamento

1. A riposo il sensore viene letto ogni 100 ms (trigger D2, echo D3).
2. Due misure consecutive a distanza **≤ 50 cm** avviano insieme i tre componenti.
   Ogni nuovo avvio alterna **Jingle Bells → Tu scendi dalle stelle → Jingle Bells**.
   Alla riaccensione o al reset della scheda, il primo brano è Jingle Bells.
3. Lo spettacolo resta occupato finché **tutti** i componenti hanno terminato.
   Tentativi di avvio durante la riproduzione vengono rifiutati, senza riavvio o coda.
4. Dopo la fine, due misure consecutive a distanza **≥ 65 cm** riarmano l'avvio.
   Anche cinque misure consecutive senza eco riarmano il sensore, per consentire
   l'allontanamento oltre il suo raggio di misura. Un eco mancante non avvia mai lo spettacolo.
5. Un nuovo avvicinamento può quindi avviare un altro ciclo. La presenza continua
   davanti al sensore non lo ripete automaticamente.

Il sensore viene letto solo a spettacolo inattivo: gli spostamenti durante la
riproduzione non vengono memorizzati. Il LED integrato segnala uno spettacolo in corso;
il monitor seriale a 9600 baud mostra disponibilità, avvio e fine.

## Concorrenza su Arduino Uno

Le tre attività usano **multitasking cooperativo**, senza thread di sistema operativo:
`loop()` richiama rapidamente ogni `update(nowMs)`, e ciascun componente esegue un
piccolo passo solo quando è arrivato il momento. I timer usano `millis()` con
sottrazione unsigned, compatibile con il rollover a 32 bit.

La struttura non richiede RTOS, allocazioni dinamiche o librerie aggiuntive.
L'impostazione segue l'esempio ufficiale Arduino
[Blink Without Delay](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay/).
Per thread preemptive bisogna progettare esplicitamente l'integrazione di un RTOS
e le sue risorse; questo progetto non implementa quel modello.

La libreria del sensore conserva `pulseIn()`: a riposo può attendere circa 6,5 ms
con la distanza massima configurata di 100 cm. Questa attesa non si verifica durante
lo spettacolo. I brevi impulsi trigger usano `delayMicroseconds()`.

## Moduli e sviluppo futuro

| File | Responsabilità |
| --- | --- |
| `src/main.cpp` | Inizializzazione, lettura sensore, ciclo principale e stato diagnostico |
| `src/ProximityTrigger.cpp` | Conferma misure, isteresi e riarmo |
| `src/ShowController.cpp` | Avvio sincronizzato, aggiornamento e fine complessiva |
| `src/CometAnimation.cpp` | Futuro movimento del servo della cometa |
| `src/ChristmasMusic.cpp` | Alternanza dei due brani sul cicalino passivo, senza attese bloccanti |
| `src/AnimatedLights.cpp` | Future animazioni delle luci |
| `include/ActivityTimer.h` | Timer comune ai segnaposto |
| `include/configuration.h` | Pin sensore, soglie, intervalli e durate |

Ogni componente espone `begin()`, `start(nowMs)`, `update(nowMs)`, `stop()` e
`isRunning()`. I tre moduli hanno una sola istanza globale ciascuno: usare un solo
`ShowController`. `begin()` viene chiamata da `setup()`; i costruttori non configurano
hardware prima dell'inizializzazione Arduino.

**Cometa e luci sono ancora segnaposto temporizzati** da 15 secondi. I punti di
implementazione sono indicati con `TODO`, senza assegnare pin o scegliere periferiche
non ancora concordate. La musica termina alla fine effettiva dello spartito.

### Cicalino passivo: due brani alternati

Collegare un piedino a **GND** e l'altro a **D5**. Il modulo usa `tone()` per generare
le frequenze in background; non usa `analogWrite()` né `delay()`.

Il primo avvicinamento avvia il ritornello di **Jingle Bells**, a 168 BPM, con brevi
stacchi fra le note e un piccolo finale acuto (circa 25 secondi). Dopo la fine e il
riarmo, il passaggio successivo avvia una strofa di **Tu scendi dalle stelle**, in
6/8, a 120 BPM (circa 39 secondi). Poi l'alternanza ricomincia.

I due spartiti sono conservati in flash su AVR, per risparmiare RAM, con pause
esplicite e legature unite in un'unica nota. La linea melodica di Tu scendi dalle
stelle è stata verificata sullo [spartito di Pianosolo](https://www.pianosolo.it/wp-content/uploads/2008/12/Tu-scendi-dalle-stelle-Spartito-per-pianoforte.pdf), usando solo la melodia tradizionale senza l'accompagnamento.

L'alternanza avanza solo per un avvio accettato: i tentativi durante la riproduzione
non saltano un brano e non vengono accodati. `stop()` conserva il prossimo brano;
`begin()` e il reset della scheda ricominciano da Jingle Bells. Non si scrive in EEPROM.

In `include/configuration.h` si possono modificare `BuzzerPin`, `MusicTempoBpm`
(Jingle Bells), `TuScendiTempoBpm` e `MusicArticulationPercent` (percentuale della
durata della nota effettivamente suonata). `ChristmasMusic::currentSong()` identifica
il brano corrente e `durationMs()` ne restituisce la durata completa, anche dopo la
fine; prima del primo avvio identifica Jingle Bells. Il monitor seriale mostra
il titolo a ogni avvio.
Un aggiornamento in ritardo salta le note già scadute senza allungare la riproduzione;
arresto e fine riportano il pin a LOW e silenziano il cicalino.

Per le future luci PWM su Uno, tenere presente che `tone()` usa Timer2 e interferisce
con PWM su D3 e D11. L'echo digitale del sensore su D3 resta compatibile.
Riferimento: [Arduino tone()](https://github.com/arduino/reference-en/blob/master/Language/Functions/Advanced%20IO/tone.adoc).

Per implementare un componente:

- Inizializzare la periferica in `begin()` e la sequenza in `start()`.
- Eseguire un solo passo breve in `update()`, usando intervalli temporali e stato
  persistente; evitare `delay()`, attese lunghe e cicli che riproducono tutta la sequenza.
- Mantenere `isRunning()` a `true` fino alla fine effettiva, incluso un eventuale
  ritorno del servo alla posizione di riposo, e sostituire il timer provvisorio
  quando è disponibile un evento reale di completamento.
- Ripristinare lo stato di riposo in `stop()` senza attese bloccanti.

## Compilazione e test

```sh
pio run -e uno
pio test -e native
```

I test sul computer verificano avvio comune, blocco dei riavvii, fine dell'ultimo
componente, arresto, isteresi, disturbi e assenza di eco, riarmo e rollover dei timer.
Verificano anche alternanza su tre passaggi, ripartenza dopo reset, frequenze,
stacchi, pause, legature, recupero di aggiornamenti in ritardo e silenzio a fine
musica per entrambi i brani, simulando le funzioni Arduino sul computer.
Non richiedono una scheda collegata. Le durate predefinite di 15 secondi per cometa
e luci fanno parte degli scenari di test: aggiornare gli scenari se cambiano.

Per caricare e osservare il firmware con la scheda collegata:

```sh
pio run -e uno -t upload
pio device monitor -b 9600
```

La compilazione e i test logici non sostituiscono la verifica fisica del sensore
e delle future periferiche.
