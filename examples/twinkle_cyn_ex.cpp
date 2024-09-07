/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#include "Cynthasine.h"

using namespace Cyn;
using namespace Notes;

int main() {

    std::vector<float> lullaby = {
        C, C, G, G, A, A, G, // "Twinkle, twinkle, little star"
        F, F, E, E, D, D, C, // "How I wonder what you are"
        G, G, F, F, E, E, D, // "Up above the world so high"
        G, G, F, F, E, E, D, // "Like a diamond in the sky"
        C, C, G, G, A, A, G, // "Twinkle, twinkle, little star"
        F, F, E, E, D, D, C  // "How I wonder what you are"
    };

    std::vector<float> lullaby_durations = {
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f
    };

    const float silence_t = 0.125f;

    for (size_t i = 0; i < lullaby.size(); ++i) {
        float note = lullaby[i];
        float note_t = lullaby_durations[i];
        Wave envelope = (Wave::cosine(1.0f / note_t, -1.0f) + 1.0f) * 0.5f;
        Wave note_wave = envelope * Wave::sine(note);
        note_wave.queue_audio(note_t);
        Wave::queue_silence(silence_t);
    }

    Wave::player().play();

    return 0;
}
