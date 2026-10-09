#include <Arduino.h>

#include "ChristmasMusic.h"
#include "configuration.h"

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

namespace {
    constexpr uint16_t Rest = 0;
    constexpr uint16_t F4 = 349;
    constexpr uint16_t G4 = 392;
    constexpr uint16_t A4 = 440;
    constexpr uint16_t Bb4 = 466;
    constexpr uint16_t C5 = 523;
    constexpr uint16_t D5 = 587;
    constexpr uint16_t E5 = 659;
    constexpr uint16_t F5 = 698;
    constexpr uint16_t G5 = 784;
    constexpr uint16_t C6 = 1047;

    struct Note {
        uint16_t frequencyHz;
        uint8_t eighthNotes;
    };

    // Ritornello di Jingle Bells (1857, pubblico dominio), con un finale scintillante.
    // 2 = un quarto, 4 = una minima, 8 = una semibreve; 0 Hz indica una pausa.
    // Su Uno lo spartito resta in flash, senza occupare la piccola RAM.
    const Note jingleBells[]
#if defined(__AVR__)
        PROGMEM
#endif
        = {
            // Jingle bells, jingle bells, jingle all the way.
            {E5, 2}, {E5, 2}, {E5, 4},
            {E5, 2}, {E5, 2}, {E5, 4},
            {E5, 2}, {G5, 2}, {C5, 2}, {D5, 2}, {E5, 8},

            // Oh, what fun it is to ride in a one-horse open sleigh.
            {F5, 2}, {F5, 2}, {F5, 2}, {F5, 2},
            {F5, 2}, {E5, 2}, {E5, 2}, {E5, 1}, {E5, 1},
            {E5, 2}, {D5, 2}, {D5, 2}, {E5, 2}, {D5, 4}, {G5, 4},
            {Rest, 2},

            // Ripresa del ritornello, con la cadenza conclusiva.
            {E5, 2}, {E5, 2}, {E5, 4},
            {E5, 2}, {E5, 2}, {E5, 4},
            {E5, 2}, {G5, 2}, {C5, 2}, {D5, 2}, {E5, 8},
            {F5, 2}, {F5, 2}, {F5, 2}, {F5, 2},
            {F5, 2}, {E5, 2}, {E5, 2}, {E5, 1}, {E5, 1},
            {G5, 2}, {G5, 2}, {F5, 2}, {D5, 2}, {C5, 8},
            {Rest, 2},

            // Piccolo saluto finale, rapido e acuto.
            {C6, 1}, {G5, 1}, {E5, 1}, {G5, 1}, {C6, 4},
        };

    // Melodia tradizionale in Fa maggiore, una strofa in 6/8.
    // Le note legate sono unite: non si interrompe il suono sulla legatura.
    // Riferimento per la linea melodica (senza accompagnamento):
    // https://www.pianosolo.it/wp-content/uploads/2008/12/Tu-scendi-dalle-stelle-Spartito-per-pianoforte.pdf
    const Note tuScendiDalleStelle[]
#if defined(__AVR__)
        PROGMEM
#endif
        = {
            // Levare e prima frase.
            {C5, 1},
            {C5, 2}, {D5, 1}, {C5, 2}, {Bb4, 1},
            {Bb4, 1}, {A4, 4}, {G4, 1},
            {A4, 1}, {Bb4, 1}, {C5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1},
            {G4, 4}, {Rest, 1}, {A4, 1},

            // Seconda frase e ripetizione.
            {G4, 2}, {A4, 1}, {Bb4, 2}, {A4, 1},
            {G4, 3}, {D5, 2}, {D5, 1},
            {C5, 1}, {D5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1}, {G4, 1},
            {A4, 4}, {Rest, 1}, {A4, 1},
            {G4, 2}, {A4, 1}, {Bb4, 2}, {A4, 1},
            {G4, 3}, {D5, 2}, {D5, 1},
            {C5, 1}, {D5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1}, {G4, 1},

            // Parte centrale, con il caratteristico andamento pastorale.
            {F4, 3}, {G4, 1}, {A4, 1}, {Bb4, 1},
            {Bb4, 1}, {A4, 1}, {Bb4, 1}, {G4, 1}, {A4, 1}, {Bb4, 1},
            {A4, 2}, {A4, 1}, {A4, 1}, {Bb4, 1}, {C5, 1},
            {Bb4, 1}, {A4, 1}, {Bb4, 1}, {G4, 1}, {A4, 1}, {Bb4, 1},
            {A4, 3}, {Rest, 2}, {G4, 1},
            {A4, 1}, {Bb4, 1}, {C5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1},
            {G4, 4}, {Rest, 1}, {A4, 1},

            // Cadenza conclusiva, ripetuta e risolta sul Fa.
            {G4, 2}, {A4, 1}, {Bb4, 2}, {A4, 1},
            {G4, 3}, {D5, 2}, {D5, 1},
            {C5, 1}, {D5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1}, {G4, 1},
            {A4, 3}, {Rest, 2}, {A4, 1},
            {G4, 2}, {A4, 1}, {Bb4, 2}, {A4, 1},
            {G4, 3}, {D5, 2}, {D5, 1},
            {C5, 1}, {D5, 1}, {C5, 1}, {Bb4, 1}, {A4, 1}, {G4, 1},
            {F4, 6},
        };

    constexpr uint8_t JingleNoteCount = sizeof(jingleBells) / sizeof(jingleBells[0]);
    constexpr uint8_t TuScendiNoteCount = sizeof(tuScendiDalleStelle) / sizeof(tuScendiDalleStelle[0]);
    static_assert(sizeof(jingleBells) / sizeof(jingleBells[0]) <= UINT8_MAX, "Spartito troppo lungo");
    static_assert(sizeof(tuScendiDalleStelle) / sizeof(tuScendiDalleStelle[0]) <= UINT8_MAX, "Spartito troppo lungo");

    ChristmasMusic::Song playingSong = ChristmasMusic::Song::JingleBells;
    ChristmasMusic::Song nextSong = ChristmasMusic::Song::JingleBells;
    const Note* melody = jingleBells;
    uint8_t noteCount = JingleNoteCount;
    uint32_t eighthNoteMs = 30000UL / Configuration::MusicTempoBpm;

    void selectSong(ChristmasMusic::Song song) {
        playingSong = song;
        if (song == ChristmasMusic::Song::JingleBells) {
            melody = jingleBells;
            noteCount = JingleNoteCount;
            eighthNoteMs = 30000UL / Configuration::MusicTempoBpm;
        }
        else {
            melody = tuScendiDalleStelle;
            noteCount = TuScendiNoteCount;
            eighthNoteMs = 30000UL / Configuration::TuScendiTempoBpm;
        }
    }

    bool running = false;
    bool sounding = false;
    uint8_t noteIndex = 0;
    uint32_t noteStartedAtMs = 0;
    uint32_t noteDurationMs = 0;
    Note currentNote = {Rest, 0};

    Note readNote(uint8_t index) {
#if defined(__AVR__)
        Note note;
        memcpy_P(&note, &melody[index], sizeof(note));
        return note;
#else
        return melody[index];
#endif
    }

    void loadCurrentNote() {
        currentNote = readNote(noteIndex);
        noteDurationMs = eighthNoteMs * currentNote.eighthNotes;
    }

    void playCurrentNote(uint32_t nowMs) {
        const uint32_t elapsedMs = static_cast<uint32_t>(nowMs - noteStartedAtMs);
        const uint32_t soundingDurationMs = noteDurationMs * Configuration::MusicArticulationPercent / 100;
        if (currentNote.frequencyHz == Rest || elapsedMs >= soundingDurationMs) {
            noTone(Configuration::BuzzerPin);
            sounding = false;
            return;
        }

        // tone() genera la frequenza in background e interrompe il suono anche
        // se un futuro componente dovesse ritardare la successiva update().
        tone(Configuration::BuzzerPin, currentNote.frequencyHz, soundingDurationMs - elapsedMs);
        sounding = true;
    }
}

namespace ChristmasMusic {
    void begin() {
        pinMode(Configuration::BuzzerPin, OUTPUT);
        stop();
        nextSong = Song::JingleBells;
        selectSong(Song::JingleBells);
    }

    void start(uint32_t nowMs) {
        if (running) {
            return;
        }

        selectSong(nextSong);
        nextSong = playingSong == Song::JingleBells ? Song::TuScendiDalleStelle : Song::JingleBells;
        noteIndex = 0;
        noteStartedAtMs = nowMs;
        running = true;
        loadCurrentNote();
        playCurrentNote(nowMs);
    }

    void update(uint32_t nowMs) {
        if (!running) {
            return;
        }

        bool noteChanged = false;
        // Recuperare aggiornamenti in ritardo senza allungare il brano o emettere
        // note ormai scadute. Il ciclo è limitato dal numero di note dello spartito.
        while (static_cast<uint32_t>(nowMs - noteStartedAtMs) >= noteDurationMs) {
            noteStartedAtMs += noteDurationMs;
            if (++noteIndex == noteCount) {
                stop();
                return;
            }

            loadCurrentNote();
            noteChanged = true;
        }

        if (noteChanged) {
            playCurrentNote(nowMs);
        }
        else if (sounding && static_cast<uint32_t>(nowMs - noteStartedAtMs) >=
                                 noteDurationMs * Configuration::MusicArticulationPercent / 100) {
            noTone(Configuration::BuzzerPin);
            sounding = false;
        }
    }

    void stop() {
        noTone(Configuration::BuzzerPin);
        digitalWrite(Configuration::BuzzerPin, LOW);
        sounding = false;
        running = false;
    }

    bool isRunning() { return running; }
    Song currentSong() { return playingSong; }

    uint32_t durationMs() {
        uint32_t totalMs = 0;
        for (uint8_t i = 0; i < noteCount; ++i) {
            totalMs += eighthNoteMs * readNote(i).eighthNotes;
        }
        return totalMs;
    }
}
