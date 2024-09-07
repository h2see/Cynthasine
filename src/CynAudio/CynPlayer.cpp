/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#include "CynPlayer.h"

#include "portaudio.h"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace Cyn {

    void sleep(double duration) {
        std::this_thread::sleep_for(std::chrono::duration<double>(duration));
    }

    class ScopedPaHandler {
    public:
        ScopedPaHandler() : _result(Pa_Initialize()) {}
        ~ScopedPaHandler() {
            if (_result == paNoError) {
                Pa_Terminate();
            }
        }
        PaError result() const { return _result; }
    private:
        PaError _result;
    };

    class Player::Impl {
    public:
        Impl(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples, std::optional<double> duration = std::nullopt, double sample_rate = 44100.0, bool do_loop = false);
        Impl(const std::vector<float>& samples, std::optional<double> duration = std::nullopt, double sample_rate = 44100.0, bool do_loop = false);
        ~Impl();

        bool start();
        bool stop();
        void wait_for_playback();
        bool play();
        void add_samples(const std::vector<float>& samples);
        void add_samples(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples);
        void clear_samples();


    private:
        PaStream* stream;
        std::vector<float> left, right;
        int channel_count;
        double sample_rate;
        bool do_loop;
        size_t num_samples_to_play, num_samples_played, idx;

        std::mutex playback_mutex, samples_mutex;
        std::condition_variable playback_condition;
        bool playback_finished;

        void set_playback_finished(bool is_finished, bool acquire_lock = true);
        bool open(std::optional<int> device_index = std::nullopt);
        bool close();
        void paEnsureInit();

        int paCallbackMethod(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);
        static int paCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
        void paStreamFinishedMethod();
        static void paStreamFinished(void* userData);

        static ScopedPaHandler paHandler;
    };

    ScopedPaHandler Player::Impl::paHandler;

    Player::Impl::Impl(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples, std::optional<double> duration, double sample_rate, bool do_loop)
        : left(left_channel_samples), right(right_channel_samples), channel_count(2), sample_rate(sample_rate), do_loop(do_loop), stream(nullptr) {
        paEnsureInit();
        if (left.size() != right.size()) {
            throw std::length_error("Left and right channel sizes must match.");
        }
        num_samples_to_play = duration ? static_cast<size_t>(this->sample_rate * duration.value()) : left.size();
        if (num_samples_to_play > left.size() && !do_loop) {
            throw std::invalid_argument("Duration exceeds available samples, and looping is disabled.");
        }
        set_playback_finished(true);
        if (!open()) {
            throw std::runtime_error("Failed to open audio stream.");
        }
    }

    Player::Impl::Impl(const std::vector<float>& samples, std::optional<double> duration, double sample_rate, bool do_loop)
        : left(samples), channel_count(1), sample_rate(sample_rate), do_loop(do_loop), stream(nullptr) {
        paEnsureInit();
        num_samples_to_play = duration ? static_cast<size_t>(this->sample_rate * duration.value()) : left.size();
        if (num_samples_to_play > left.size() && !do_loop) {
            throw std::invalid_argument("Duration exceeds available samples, and looping is disabled.");
        }
        set_playback_finished(true);
        if (!open()) {
            throw std::runtime_error("Failed to open audio stream.");
        }
    }

    Player::Impl::~Impl() {
        if (!close()) {
            std::cerr << "Error: Failed to close the stream during destruction." << std::endl;
        }
    }

    bool Player::Impl::start() {
        std::lock_guard<std::mutex> lock(playback_mutex);
        if (!stream || !playback_finished) return false;

        PaError err = Pa_StartStream(stream);
        if (err == paNoError) {
            set_playback_finished(false, false);
            return true;
        }
        return false;
    }

    bool Player::Impl::stop() {
        std::lock_guard<std::mutex> lock(playback_mutex);
        if (!stream || playback_finished) return false;

        PaError err = Pa_StopStream(stream);
        if (err == paNoError) {
            set_playback_finished(true, false);
            return true;
        }
        return false;
    }

    void Player::Impl::wait_for_playback() {
        std::unique_lock<std::mutex> lock(playback_mutex);
        playback_condition.wait(lock, [&]() { return playback_finished; });
    }

    bool Player::Impl::play() {
        if (!start()) return false;
        wait_for_playback();
        return stop();
    }

    void Player::Impl::add_samples(const std::vector<float>& samples) {
        std::lock_guard<std::mutex> lock(samples_mutex);
        if (channel_count != 1) {
            throw std::runtime_error("Mono output configured; input only one sample vector.");
        }
        num_samples_to_play += samples.size();
        left.insert(left.end(), samples.begin(), samples.end());
    }

    void Player::Impl::add_samples(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples) {
        if (left_channel_samples.size() != right_channel_samples.size()) {
            throw std::length_error("Left and right channel sizes must match.");
        }
        std::lock_guard<std::mutex> lock(samples_mutex);
        if (channel_count != 2) {
            throw std::runtime_error("Stereo output configured; input two sample vectors.");
        }
        num_samples_to_play += left_channel_samples.size();
        left.insert(left.end(), left_channel_samples.begin(), left_channel_samples.end());
        right.insert(right.end(), right_channel_samples.begin(), right_channel_samples.end());
    }

    void Player::Impl::clear_samples() {
        stop();
        std::lock_guard<std::mutex> lock(samples_mutex);
        left.clear();
        right.clear();
    }

    void Player::Impl::set_playback_finished(bool is_finished, bool acquire_lock) {
        if (acquire_lock) {
            std::lock_guard<std::mutex> lock(playback_mutex);
        }
        playback_finished = is_finished;
        if (is_finished) {
            idx = num_samples_played = 0;
        }
        playback_condition.notify_one();
    }

    bool Player::Impl::open(std::optional<int> device_index) {
        PaDeviceIndex index = device_index.value_or(Pa_GetDefaultOutputDevice());

        PaStreamParameters outputParameters = {};
        outputParameters.device = index;
        if (outputParameters.device == paNoDevice) return false;

        outputParameters.channelCount = channel_count;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;

        PaError err = Pa_OpenStream(
            &stream,
            nullptr,
            &outputParameters,
            sample_rate,
            paFramesPerBufferUnspecified,
            paNoFlag,
            &Impl::paCallback,
            this
        );

        if (err == paNoError) {
            err = Pa_SetStreamFinishedCallback(stream, &Impl::paStreamFinished);
        }

        if (err != paNoError) {
            Pa_CloseStream(stream);
            stream = nullptr;
            return false;
        }

        return true;
    }

    bool Player::Impl::close() {
        if (!stream) return false;

        PaError err = Pa_CloseStream(stream);
        stream = nullptr;
        return (err == paNoError);
    }

    void Player::Impl::paEnsureInit() {
        if (paHandler.result() != paNoError) {
            throw std::runtime_error("Failed to initialize PortAudio: " + std::string(Pa_GetErrorText(paHandler.result())));
        }
    }

    int Player::Impl::paCallbackMethod(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags) {
        std::lock_guard<std::mutex> lock(samples_mutex);
        float* out = static_cast<float*>(outputBuffer);

        for (size_t i = 0; i < framesPerBuffer; ++i) {
            *out++ = left[idx];
            if (channel_count == 2) *out++ = right[idx];
            idx++;
            num_samples_played++;
            if (num_samples_played >= num_samples_to_play) {
                set_playback_finished(true);
                return paComplete;
            }
            if (idx >= left.size()) {
                if (do_loop) {
                    idx -= left.size();
                }
                else {
                    std::fill(out, out + (framesPerBuffer - i) * channel_count, 0.0f);
                    set_playback_finished(true);
                    return paComplete;
                }
            }
        }
        return paContinue;
    }

    int Player::Impl::paCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void* userData) {
        return static_cast<Impl*>(userData)->paCallbackMethod(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
    }

    void Player::Impl::paStreamFinishedMethod() {}

    void Player::Impl::paStreamFinished(void* userData) {
        static_cast<Impl*>(userData)->paStreamFinishedMethod();
    }

    Player::Player(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples, std::optional<double> duration, double sample_rate, bool do_loop)
        : pImpl(std::make_unique<Impl>(left_channel_samples, right_channel_samples, duration, sample_rate, do_loop)) {}

    Player::Player(const std::vector<float>& samples, std::optional<double> duration, double sample_rate, bool do_loop)
        : pImpl(std::make_unique<Impl>(samples, duration, sample_rate, do_loop)) {}

    Player::~Player() = default;

    bool Player::start() { return pImpl->start(); }
    bool Player::stop() { return pImpl->stop(); }
    void Player::wait_for_playback() { pImpl->wait_for_playback(); }
    bool Player::play() { return pImpl->play(); }

    void Player::add_samples(const std::vector<float>& samples) {
        pImpl->add_samples(samples);
    }
    void Player::add_samples(const std::vector<float>& left_channel_samples, const std::vector<float>& right_channel_samples) {
        pImpl->add_samples(left_channel_samples, right_channel_samples);
    }

    void Player::clear_samples() { pImpl->clear_samples(); }

    Player player_44100(std::vector<float>{}, std::nullopt, 44100.0, false);
    Player player_44800(std::vector<float>{}, std::nullopt, 44800.0, false);

} // namespace Cyn
