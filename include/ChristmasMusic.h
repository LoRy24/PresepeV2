#ifndef PRESEPEV2_CHRISTMAS_MUSIC_H
#define PRESEPEV2_CHRISTMAS_MUSIC_H

#include <stdint.h>

namespace ChristmasMusic {
    enum class Song : uint8_t { JingleBells, TuScendiDalleStelle };

    void begin();
    void start(uint32_t nowMs);
    void update(uint32_t nowMs);
    void stop();
    bool isRunning();
    Song currentSong();
    // Durata del brano corrente (o di Jingle Bells prima del primo avvio).
    uint32_t durationMs();
}

#endif
