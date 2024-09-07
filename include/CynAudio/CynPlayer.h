/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#ifndef CYN_PLAYER_H
#define CYN_PLAYER_H

#include <optional>
#include <vector>
#include <memory>

namespace Cyn {

    void sleep(double duration);

    class Player {
    public:
        // Constructor for stereo playback (left and right channels).
        Player(const std::vector<float>& left_channel_samples,
            const std::vector<float>& right_channel_samples,
            std::optional<double> duration = std::nullopt,
            double sample_rate = 44100.0,
            bool do_loop = false
            );

        // Constructor for mono playback (single channel).
        Player(const std::vector<float>& samples,
            std::optional<double> duration = std::nullopt,
            double sample_rate = 44100.0,
            bool do_loop = false
            );

        // Destructor
        ~Player();

        // Start playback of the audio samples.
        bool start();

        // Stop playback of the audio samples.
        bool stop();

        // Wait until playback is finished.
        void wait_for_playback();

        // Play the audio samples (start playback and block until finished).
        bool play();

        void add_samples(const std::vector<float>& samples);
        void add_samples(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples);
        void clear_samples();

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl; // Pointer to implementation
    };

    extern Cyn::Player player_44100;
    extern Cyn::Player player_44800;

} // namespace Cyn

#endif // CYN_PLAYER_H
