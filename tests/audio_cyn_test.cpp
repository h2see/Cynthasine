/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#include "Cynthasine.h"
#include "gtest/gtest.h"

using namespace Cyn;
namespace fs = std::filesystem;

class WaveTest : public ::testing::Test {
protected:
    fs::path source_dir;
    fs::path audio_dir;
    fs::path misc_output_dir;

    WaveTest() : source_dir(SOURCE_DIR) {
        source_dir = source_dir.lexically_normal();
        audio_dir = source_dir / "tests" / "test_data" / "audio";
        misc_output_dir = source_dir / "tests" / "test_data" / "misc_output";
        if (!fs::exists(misc_output_dir)) {
            if (!fs::create_directories(misc_output_dir)) {
                throw std::runtime_error("Failed to create the tests/test_data/misc_output directory.");
            }
        }
        validate_directory(source_dir, std::string(SOURCE_DIR));
        validate_directory(audio_dir, "tests/test_data/audio");
        validate_directory(misc_output_dir, "tests/test_data/test_signals/misc_output");
    }

private:
    void validate_directory(const fs::path& path, const std::string& name) {
        if (!(fs::exists(path) && fs::is_directory(path))) {
            std::cerr << name << " does not exist or is not a directory!" << std::endl;
        }
    }
};

TEST_F(WaveTest, LoadPlaySamples) {
    std::vector<std::string> header;
    Eigen::ArrayXXf uke_signal = load_from_csv<float>(audio_dir / "Ukulele.csv", &header);
    Wave uke = Wave::from_samples(uke_signal, 44100.0f, -1.0f);

    std::vector<float> lullaby = {
    Note::C, Note::C, Note::G, Note::G, Note::A, Note::A, Note::G,
    Note::F, Note::F, Note::E, Note::E, Note::D, Note::D, Note::C,
    Note::G, Note::G, Note::F, Note::F, Note::E, Note::E, Note::D,
    Note::G, Note::G, Note::F, Note::F, Note::E, Note::E, Note::D,
    Note::C, Note::C, Note::G, Note::G, Note::A, Note::A, Note::G,
    Note::F, Note::F, Note::E, Note::E, Note::D, Note::D, Note::C
    };

    std::vector<float> lullaby_durations = {
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f,
        0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f
    };

    const float silence_t = 0.25;

    for (size_t i = 0; i < lullaby.size(); ++i) {
        float note = lullaby[i];
        float note_t = lullaby_durations[i] * 2;
        float freq_mod = note / 440.0f;
        Wave note_wave = uke * (Wave::cosine(0.5f / note_t) + 1.0f) * 0.5f;
        note_wave.freq() *= freq_mod;
        Wave filtered_note_wave = note_wave.filter(note_wave.amp().abs() > 1e-3f && note_wave.freq().abs() <= 22050.0f);
        Eigen::ArrayXf samps = filtered_note_wave.samples(note_t);
        std::vector<float> samps_vec(samps.data(), samps.data() + samps.size());
        Wave::player().add_samples(samps_vec);
        Wave::queue_silence(silence_t);
    }
    Wave::player().play();
}
