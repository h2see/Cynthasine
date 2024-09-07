/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

Eigen::ArrayX<WaveT> samples_audio(WaveT duration, bool filter_high_freqs = true, bool remove_bias = true, bool scale_samples = true, std::optional<WaveT> tolerance = std::nullopt) const {

    Eigen::ArrayXb to_keep;
    Eigen::ArrayX<WaveT> abs_freq;
    bool to_keep_init = false;
    if (filter_high_freqs) {
        abs_freq = freq().abs();
        to_keep = abs_freq <= nyquist_freq();
        to_keep_init = true;
    }

    if (remove_bias) {
        if (to_keep_init) {
            to_keep = to_keep && (abs_freq > tolerance.value_or(TOLERANCE));
        }
        else {
            to_keep = freq().abs() > tolerance.value_or(TOLERANCE);
            to_keep_init = true;
        }
    }

    Eigen::ArrayX<WaveT> result_audio_samples;
    if (to_keep_init) {
        WaveArray<WaveT> preprocessed_wave = filter(to_keep);
        result_audio_samples = preprocessed_wave.samples(duration, SAMPLE_RATE);
    }
    else {
        result_audio_samples = samples(duration, SAMPLE_RATE);
    }


    if (scale_samples) {
        WaveT min_sample = result_audio_samples.minCoeff();
        WaveT max_sample = result_audio_samples.maxCoeff();
        WaveT abs_min_sample = std::abs(min_sample);
        WaveT abs_max_sample = std::abs(max_sample);
        if (abs_min_sample > 1.0f && abs_max_sample > 1.0f) {
            result_audio_samples = scale<Eigen::ArrayX<WaveT>, WaveT>(result_audio_samples, -1.0f, 1.0f, std::optional<WaveT>(min_sample), std::optional<WaveT>(max_sample));
        }
        else if (abs_min_sample > 1.0f) {
            result_audio_samples = scale<Eigen::ArrayX<WaveT>, WaveT>(result_audio_samples, -1.0f, max_sample, std::optional<WaveT>(min_sample), std::optional<WaveT>(max_sample));
        }
        else if (abs_max_sample > 1.0f) {
            result_audio_samples = scale<Eigen::ArrayX<WaveT>, WaveT>(result_audio_samples, min_sample, 1.0f, std::optional<WaveT>(min_sample), std::optional<WaveT>(max_sample));
        }
    }

    return std::move(result_audio_samples);

}


inline static Player& player() {
    if (isclose(SAMPLE_RATE, 44100.0f, TOLERANCE)) {
        return player_44100;
    }
    else if (isclose(SAMPLE_RATE, 44800.0f, TOLERANCE)) {
        return player_44800;
    }
    else {
        throw std::runtime_error("No player found for the configured sample rate. Please use 44100 or 44800 [Hz].");
    }
}


inline void queue_audio(WaveT duration, bool filter_high_freqs = true, bool remove_bias = true, bool scale_samples = true, std::optional<WaveT> tolerance = std::nullopt) const {
    Eigen::ArrayX<WaveT> samples_to_queue = samples_audio(duration, filter_high_freqs, remove_bias, scale_samples, tolerance);
    if constexpr (std::is_same_v<WaveT, float>) {
        // Directly copy data when WaveT is float
        std::vector<float> vec_samples_to_queue(samples_to_queue.data(), samples_to_queue.data() + samples_to_queue.size());
        player().add_samples(vec_samples_to_queue);
    }
    else {
        // Convert each sample to float otherwise
        std::vector<float> vec_samples_to_queue(samples_to_queue.size());
        std::transform(samples_to_queue.data(), samples_to_queue.data() + samples_to_queue.size(), vec_samples_to_queue.begin(),
            [](WaveT sample) { return static_cast<float>(sample); });
        player().add_samples(vec_samples_to_queue);
    }
}


inline static void queue_silence(WaveT duration) {
    std::vector<float> vec_silence(static_cast<size_t>(SAMPLE_RATE * duration), 0.0f);
    player().add_samples(vec_silence);
}


#ifdef CYN_AUDIO_WAVE_ADDON
#include CYN_AUDIO_WAVE_ADDON
#endif // CYN_AUDIO_WAVE_ADDON
