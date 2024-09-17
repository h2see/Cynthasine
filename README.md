# Cynthasine

Cynthasine is a C++ Eigen-based signal processing library and audio synthesizer.

## Quickstart
- Compile using the CMake build system
- See examples/twinkle_cyn_ex.cpp for basic usage

**examples/twinkle_cyn_ex.cpp:**
```cpp
/*
 * Except where otherwise noted, Cynthasine � 2024 by https://github.com/h2see is licensed under Creative
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
```

## License Information

Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

For a comprehensive explanation of the licenses governing the Cynthasine © 2024 project,
including details on specific files and their applicable licenses, please refer to the
accompanying NOTICE document. This document outlines the licensing terms under which
the project is distributed and must be adhered to in any derivative works or distributions.

The NOTICE document, including all of its contents, must be retained in the root directory of any
distribution or derivative of Cynthasine © 2024. Failure to include this file does not exempt you
from the terms of the licenses governing this project.
