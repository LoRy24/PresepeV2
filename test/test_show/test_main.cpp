#include <unity.h>
#include <Arduino.h>

#include "ActivityTimer.h"
#include "AnimatedLights.h"
#include "ChristmasMusic.h"
#include "CometAnimation.h"
#include "ProximityTrigger.h"
#include "ShowController.h"
#include "configuration.h"

void setUp() {
    ChristmasMusic::begin();
    FakeArduino::reset();
}
void tearDown() { ChristmasMusic::stop(); }

namespace {
    bool confirmDistance(ProximityTrigger& trigger, uint32_t distanceCm) {
        bool activated = false;
        for (uint8_t i = 0; i < Configuration::RequiredConsecutiveReadings; ++i) {
            activated = trigger.update(distanceCm) || activated;
        }
        return activated;
    }

    void test_activation_requires_consecutive_near_readings() {
        ProximityTrigger trigger;
        for (uint8_t i = 1; i < Configuration::RequiredConsecutiveReadings; ++i) {
            TEST_ASSERT_FALSE(trigger.update(30));
        }
        TEST_ASSERT_TRUE(trigger.update(30));
    }

    void test_presence_does_not_repeatedly_activate() {
        ProximityTrigger trigger;
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
        for (uint8_t i = 0; i < 20; ++i) {
            TEST_ASSERT_FALSE(trigger.update(30));
        }
    }

    void test_noise_breaks_activation_confirmation() {
        ProximityTrigger trigger;
        TEST_ASSERT_FALSE(trigger.update(30));
        TEST_ASSERT_FALSE(trigger.update(Configuration::ActivationDistanceCm + 1));
        TEST_ASSERT_FALSE(trigger.update(30));
        TEST_ASSERT_FALSE(trigger.update(0));
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
    }

    void test_hysteresis_and_threshold_boundaries() {
        ProximityTrigger trigger;
        TEST_ASSERT_TRUE(confirmDistance(trigger, Configuration::ActivationDistanceCm));
        TEST_ASSERT_FALSE(confirmDistance(trigger, Configuration::RearmDistanceCm - 1));
        TEST_ASSERT_FALSE(confirmDistance(trigger, 30));
        TEST_ASSERT_FALSE(confirmDistance(trigger, Configuration::RearmDistanceCm));
        TEST_ASSERT_TRUE(confirmDistance(trigger, Configuration::ActivationDistanceCm));
    }

    void test_rearm_requires_consecutive_far_readings() {
        ProximityTrigger trigger;
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
        TEST_ASSERT_FALSE(trigger.update(80));
        TEST_ASSERT_FALSE(trigger.update(30));
        TEST_ASSERT_FALSE(trigger.update(80));
        TEST_ASSERT_FALSE(trigger.update(30));
        TEST_ASSERT_FALSE(confirmDistance(trigger, 30));
        TEST_ASSERT_FALSE(confirmDistance(trigger, 80));
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
    }

    void test_missing_echo_never_activates() {
        ProximityTrigger trigger;
        for (uint8_t i = 0; i < 20; ++i) {
            TEST_ASSERT_FALSE(trigger.update(0));
        }
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
    }

    void test_brief_echo_loss_does_not_rearm() {
        ProximityTrigger trigger;
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
        for (uint8_t i = 1; i < Configuration::NoEchoReadingsToRearm; ++i) {
            TEST_ASSERT_FALSE(trigger.update(0));
        }
        TEST_ASSERT_FALSE(confirmDistance(trigger, 30));
    }

    void test_sustained_echo_loss_rearms_for_next_visitor() {
        ProximityTrigger trigger;
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
        for (uint8_t i = 0; i < Configuration::NoEchoReadingsToRearm; ++i) {
            TEST_ASSERT_FALSE(trigger.update(0));
        }
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
    }

    void test_out_of_range_readings_are_ignored() {
        ProximityTrigger trigger;
        TEST_ASSERT_FALSE(confirmDistance(trigger, Configuration::SensorMaximumDistanceCm + 1));
        TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
        TEST_ASSERT_FALSE(confirmDistance(trigger, UINT32_MAX));
        TEST_ASSERT_FALSE(confirmDistance(trigger, 30));
    }

    void test_show_starts_all_components_and_rejects_restart() {
        ShowController show;
        show.begin();
        TEST_ASSERT_FALSE(show.isRunning());
        TEST_ASSERT_TRUE(show.start(100));
        TEST_ASSERT_TRUE(CometAnimation::isRunning());
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        TEST_ASSERT_TRUE(AnimatedLights::isRunning());
        TEST_ASSERT_FALSE(show.start(1000));

        // Un tentativo di riavvio non deve prolungare lo spettacolo originale.
        show.update(100 + ChristmasMusic::durationMs());
        TEST_ASSERT_FALSE(show.isRunning());
    }

    void test_show_waits_for_last_component() {
        ShowController show;
        show.begin();
        TEST_ASSERT_TRUE(show.start(100));
        show.update(15099);
        TEST_ASSERT_TRUE(CometAnimation::isRunning());
        TEST_ASSERT_TRUE(AnimatedLights::isRunning());
        show.update(15100);
        TEST_ASSERT_FALSE(CometAnimation::isRunning());
        TEST_ASSERT_FALSE(AnimatedLights::isRunning());
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        TEST_ASSERT_TRUE(show.isRunning());
        TEST_ASSERT_FALSE(show.start(15100));
        const uint32_t endedAt = 100 + ChristmasMusic::durationMs();
        show.update(endedAt - 1);
        TEST_ASSERT_TRUE(show.isRunning());
        show.update(endedAt);
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        TEST_ASSERT_FALSE(show.isRunning());
        TEST_ASSERT_TRUE(show.start(endedAt + 1));
    }

    void test_stop_cleans_up_all_components_and_allows_restart() {
        ShowController show;
        show.begin();
        TEST_ASSERT_TRUE(show.start(100));
        show.stop();
        TEST_ASSERT_FALSE(show.isRunning());
        TEST_ASSERT_FALSE(CometAnimation::isRunning());
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        TEST_ASSERT_FALSE(AnimatedLights::isRunning());
        TEST_ASSERT_TRUE(show.start(200));
        show.begin();
        TEST_ASSERT_FALSE(show.isRunning());
        TEST_ASSERT_FALSE(CometAnimation::isRunning());
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        TEST_ASSERT_FALSE(AnimatedLights::isRunning());
    }

    void test_show_timers_survive_millis_rollover() {
        ShowController show;
        show.begin();
        const uint32_t startedAt = UINT32_MAX - 1000;
        TEST_ASSERT_TRUE(show.start(startedAt));
        show.update(static_cast<uint32_t>(startedAt + ChristmasMusic::durationMs() - 1));
        TEST_ASSERT_TRUE(show.isRunning());
        show.update(static_cast<uint32_t>(startedAt + ChristmasMusic::durationMs()));
        TEST_ASSERT_FALSE(show.isRunning());
    }

    void test_zero_duration_and_cancelled_timers_stay_idle() {
        ActivityTimer timer;
        timer.start(100, 0);
        TEST_ASSERT_FALSE(timer.isRunning());
        timer.start(100, 1000);
        timer.stop();
        timer.update(101);
        TEST_ASSERT_FALSE(timer.isRunning());
    }

    void test_music_initializes_pin_five_and_starts_with_e5() {
        ChristmasMusic::begin();
        TEST_ASSERT_EQUAL_UINT8(5, FakeArduino::state().configuredPin);
        TEST_ASSERT_EQUAL_UINT8(OUTPUT, FakeArduino::state().mode);
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
        ChristmasMusic::start(100);
        TEST_ASSERT_EQUAL_UINT8(5, FakeArduino::state().tonePin);
        TEST_ASSERT_EQUAL_UINT16(659, FakeArduino::state().frequencyHz);
        TEST_ASSERT_TRUE(FakeArduino::state().durationMs > 0);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
    }

    void test_repeated_notes_have_silent_gaps() {
        const uint32_t quarterMs = 2 * (30000UL / Configuration::MusicTempoBpm);
        const uint32_t soundMs = quarterMs * Configuration::MusicArticulationPercent / 100;
        ChristmasMusic::begin();
        ChristmasMusic::start(100);
        ChristmasMusic::update(100 + soundMs - 1);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        ChristmasMusic::update(100 + soundMs);
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        ChristmasMusic::update(100 + quarterMs);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        TEST_ASSERT_EQUAL_UINT16(659, FakeArduino::state().frequencyHz);
        TEST_ASSERT_EQUAL_UINT(2, FakeArduino::state().toneCalls);
    }

    void test_music_catches_up_to_correct_pitch_after_delayed_update() {
        const uint32_t quarterMs = 2 * (30000UL / Configuration::MusicTempoBpm);
        ChristmasMusic::begin();
        ChristmasMusic::start(100);
        ChristmasMusic::update(100 + 9 * quarterMs + 10);
        TEST_ASSERT_EQUAL_UINT16(784, FakeArduino::state().frequencyHz);
        TEST_ASSERT_EQUAL_UINT(2, FakeArduino::state().toneCalls);
        TEST_ASSERT_EQUAL_UINT32(quarterMs * Configuration::MusicArticulationPercent / 100 - 10,
                                 FakeArduino::state().durationMs);
    }

    void test_music_plays_a_silent_pause_between_choruses() {
        const uint32_t quarterMs = 2 * (30000UL / Configuration::MusicTempoBpm);
        ChristmasMusic::begin();
        ChristmasMusic::start(100);
        ChristmasMusic::update(100 + 32 * quarterMs);
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        ChristmasMusic::update(100 + 33 * quarterMs);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        TEST_ASSERT_EQUAL_UINT16(659, FakeArduino::state().frequencyHz);
    }

    void test_music_ends_in_silence_and_can_restart() {
        ChristmasMusic::begin();
        ChristmasMusic::start(100);
        ChristmasMusic::update(100 + ChristmasMusic::durationMs() + 1000);
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
        TEST_ASSERT_EQUAL_UINT8(Configuration::BuzzerPin, FakeArduino::state().writtenPin);
        TEST_ASSERT_EQUAL_UINT8(LOW, FakeArduino::state().writtenValue);
        ChristmasMusic::start(50000);
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        TEST_ASSERT_EQUAL_UINT16(523, FakeArduino::state().frequencyHz);
        ChristmasMusic::stop();
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
    }

    void test_music_rejects_restart_and_stays_silent_when_idle() {
        ChristmasMusic::begin();
        ChristmasMusic::start(100);
        ChristmasMusic::start(1000);
        TEST_ASSERT_EQUAL_UINT(1, FakeArduino::state().toneCalls);
        ChristmasMusic::update(100 + ChristmasMusic::durationMs());
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        const unsigned int toneCalls = FakeArduino::state().toneCalls;
        ChristmasMusic::update(100000);
        TEST_ASSERT_EQUAL_UINT(toneCalls, FakeArduino::state().toneCalls);
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
    }

    void test_music_alternates_only_between_accepted_starts() {
        ChristmasMusic::start(100);
        TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == ChristmasMusic::Song::JingleBells);
        const uint32_t jingleDuration = ChristmasMusic::durationMs();
        ChristmasMusic::start(1000); // Rifiutato: non deve consumare il prossimo brano.
        ChristmasMusic::update(100 + jingleDuration);
        ChristmasMusic::start(50000);
        TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == ChristmasMusic::Song::TuScendiDalleStelle);
        TEST_ASSERT_EQUAL_UINT16(523, FakeArduino::state().frequencyHz);
        TEST_ASSERT_EQUAL_UINT32(157 * (30000UL / Configuration::TuScendiTempoBpm), ChristmasMusic::durationMs());
        ChristmasMusic::update(50000 + ChristmasMusic::durationMs());
        ChristmasMusic::start(100000);
        TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == ChristmasMusic::Song::JingleBells);
        TEST_ASSERT_EQUAL_UINT16(659, FakeArduino::state().frequencyHz);
        TEST_ASSERT_EQUAL_UINT32(jingleDuration, ChristmasMusic::durationMs());
    }

    void test_begin_resets_alternation_but_stop_preserves_next_song() {
        ChristmasMusic::start(100);
        ChristmasMusic::stop();
        ChristmasMusic::start(200);
        TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == ChristmasMusic::Song::TuScendiDalleStelle);
        ChristmasMusic::begin();
        ChristmasMusic::start(300);
        TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == ChristmasMusic::Song::JingleBells);
    }

    void test_tu_scendi_plays_opening_tied_notes_and_rest_at_its_own_tempo() {
        ChristmasMusic::start(0);
        ChristmasMusic::stop();
        ChristmasMusic::start(100);
        const uint32_t eighthMs = 30000UL / Configuration::TuScendiTempoBpm;
        TEST_ASSERT_EQUAL_UINT16(523, FakeArduino::state().frequencyHz);
        ChristmasMusic::update(100 + 3 * eighthMs);
        TEST_ASSERT_EQUAL_UINT16(587, FakeArduino::state().frequencyHz);
        ChristmasMusic::update(100 + 8 * eighthMs);
        TEST_ASSERT_EQUAL_UINT16(440, FakeArduino::state().frequencyHz);
        TEST_ASSERT_EQUAL_UINT32(4 * eighthMs * Configuration::MusicArticulationPercent / 100,
                                 FakeArduino::state().durationMs);
        const unsigned int toneCalls = FakeArduino::state().toneCalls;
        ChristmasMusic::update(100 + 10 * eighthMs);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        TEST_ASSERT_EQUAL_UINT(toneCalls, FakeArduino::state().toneCalls);
        ChristmasMusic::update(100 + 23 * eighthMs);
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        ChristmasMusic::update(100 + 24 * eighthMs);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        TEST_ASSERT_EQUAL_UINT16(440, FakeArduino::state().frequencyHz);
    }

    void test_tu_scendi_ends_on_f4_and_survives_timer_rollover() {
        ChristmasMusic::start(0);
        ChristmasMusic::stop();
        const uint32_t startedAt = UINT32_MAX - 1000;
        ChristmasMusic::start(startedAt);
        const uint32_t duration = ChristmasMusic::durationMs();
        const uint32_t finalNoteMs = 6 * (30000UL / Configuration::TuScendiTempoBpm);
        ChristmasMusic::update(static_cast<uint32_t>(startedAt + duration - finalNoteMs));
        TEST_ASSERT_EQUAL_UINT16(349, FakeArduino::state().frequencyHz);
        TEST_ASSERT_TRUE(FakeArduino::state().sounding);
        ChristmasMusic::update(static_cast<uint32_t>(startedAt + duration - 1));
        TEST_ASSERT_TRUE(ChristmasMusic::isRunning());
        ChristmasMusic::update(static_cast<uint32_t>(startedAt + duration));
        TEST_ASSERT_FALSE(ChristmasMusic::isRunning());
        TEST_ASSERT_FALSE(FakeArduino::state().sounding);
    }

    void test_three_proximity_cycles_alternate_songs_without_queued_restarts() {
        ShowController show;
        ProximityTrigger trigger;
        show.begin();
        uint32_t startedAt = 100;
        for (uint8_t cycle = 0; cycle < 3; ++cycle) {
            TEST_ASSERT_TRUE(confirmDistance(trigger, 30));
            TEST_ASSERT_TRUE(show.start(startedAt));
            const ChristmasMusic::Song expectedSong = cycle == 1 ? ChristmasMusic::Song::TuScendiDalleStelle
                                                                 : ChristmasMusic::Song::JingleBells;
            TEST_ASSERT_TRUE(ChristmasMusic::currentSong() == expectedSong);
            const uint32_t duration = ChristmasMusic::durationMs();
            TEST_ASSERT_FALSE(show.start(startedAt + 1000));
            show.update(startedAt + duration - 1);
            TEST_ASSERT_TRUE(show.isRunning());
            show.update(startedAt + duration);
            TEST_ASSERT_FALSE(show.isRunning());
            TEST_ASSERT_FALSE(confirmDistance(trigger, 30));
            TEST_ASSERT_FALSE(confirmDistance(trigger, 80));
            startedAt += duration + 1000;
        }
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_activation_requires_consecutive_near_readings);
    RUN_TEST(test_presence_does_not_repeatedly_activate);
    RUN_TEST(test_noise_breaks_activation_confirmation);
    RUN_TEST(test_hysteresis_and_threshold_boundaries);
    RUN_TEST(test_rearm_requires_consecutive_far_readings);
    RUN_TEST(test_missing_echo_never_activates);
    RUN_TEST(test_brief_echo_loss_does_not_rearm);
    RUN_TEST(test_sustained_echo_loss_rearms_for_next_visitor);
    RUN_TEST(test_out_of_range_readings_are_ignored);
    RUN_TEST(test_show_starts_all_components_and_rejects_restart);
    RUN_TEST(test_show_waits_for_last_component);
    RUN_TEST(test_stop_cleans_up_all_components_and_allows_restart);
    RUN_TEST(test_show_timers_survive_millis_rollover);
    RUN_TEST(test_zero_duration_and_cancelled_timers_stay_idle);
    RUN_TEST(test_music_initializes_pin_five_and_starts_with_e5);
    RUN_TEST(test_repeated_notes_have_silent_gaps);
    RUN_TEST(test_music_catches_up_to_correct_pitch_after_delayed_update);
    RUN_TEST(test_music_plays_a_silent_pause_between_choruses);
    RUN_TEST(test_music_ends_in_silence_and_can_restart);
    RUN_TEST(test_music_rejects_restart_and_stays_silent_when_idle);
    RUN_TEST(test_music_alternates_only_between_accepted_starts);
    RUN_TEST(test_begin_resets_alternation_but_stop_preserves_next_song);
    RUN_TEST(test_tu_scendi_plays_opening_tied_notes_and_rest_at_its_own_tempo);
    RUN_TEST(test_tu_scendi_ends_on_f4_and_survives_timer_rollover);
    RUN_TEST(test_three_proximity_cycles_alternate_songs_without_queued_restarts);
    return UNITY_END();
}
