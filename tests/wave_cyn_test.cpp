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
    fs::path truth_signals_dir;
    fs::path test_signals_dir;
    fs::path misc_output_dir;
    std::vector<Wave> random_waves;
    float tolerance = 1e-3f;

    WaveTest() : source_dir(SOURCE_DIR) {
        source_dir = source_dir.lexically_normal();
        truth_signals_dir = source_dir / "tests" / "test_data" / "truth_signals";
        test_signals_dir = source_dir / "tests" / "test_data" / "test_signals";
        misc_output_dir = source_dir / "tests" / "test_data" / "misc_output";
        if (!fs::exists(test_signals_dir)) {
            if (!fs::create_directories(test_signals_dir)) {
                throw std::runtime_error("Failed to create the tests/test_data/test_signals directory.");
            }
        }
        if (!fs::exists(misc_output_dir)) {
            if (!fs::create_directories(misc_output_dir)) {
                throw std::runtime_error("Failed to create the tests/test_data/misc_output directory.");
            }
        }
        validate_directory(source_dir, std::string(SOURCE_DIR));
        validate_directory(truth_signals_dir, "tests/test_data/truth_signals");
        validate_directory(test_signals_dir, "tests/test_data/test_signals");
        validate_directory(misc_output_dir, "tests/test_data/test_signals/misc_output");

        fs::path random_waves_dir = source_dir / "tests" / "test_data" / "random_waves";
        validate_directory(random_waves_dir, "tests/test_data/random_waves");
        load_random_waves(random_waves_dir);
    }

private:
    void validate_directory(const fs::path& path, const std::string& name) {
        if (!(fs::exists(path) && fs::is_directory(path))) {
            std::cerr << name << " does not exist or is not a directory!" << std::endl;
        }
    }

    void load_random_waves(const fs::path& dir) {
        if (fs::exists(dir) && fs::is_directory(dir)) {
            for (const auto& fpath : fs::directory_iterator(dir)) {
                random_waves.emplace_back(Wave::from_csv(fpath));
            }
        }
        else {
            std::cerr << dir << " does not exist or is not a directory!" << std::endl;
        }
    }
public:
    void compare_wave_result_with_truth_signal(const Wave& test_wave_result, const std::string& truth_signal_filename) const {
        std::vector<std::string> header;
        Eigen::ArrayXXf truth_signal = load_from_csv<float>(truth_signals_dir / truth_signal_filename, &header);
        auto truth_signal_t = truth_signal.col(0);
        auto truth_signal_s = truth_signal.col(1);
        float duration = truth_signal_t(truth_signal_t.size() - 1) - truth_signal_t(0);
        float sample_rate = truth_signal_t.size() / duration;
        Eigen::ArrayXf test_signal_t;
        Eigen::ArrayXf test_signal_s = test_wave_result.samples(duration, sample_rate, &test_signal_t);
        test_wave_result.to_csv_samples(test_signals_dir / truth_signal_filename, duration, sample_rate);
        EXPECT_TRUE(test_signal_t.isApprox(truth_signal_t, tolerance));
        EXPECT_TRUE(test_signal_s.isApprox(truth_signal_s, tolerance));
    }
};

TEST_F(WaveTest, FromSamplesFourier) {

    Eigen::ArrayXf r_0(8);
    r_0 << 2.f, 1.f, 2.f, 3.f, 1.f, 1.f, 0.f, 3.f;
    Eigen::ArrayX<std::complex<float>> ft_0 = FFT::r2c(r_0);
    FFT::make_hermitian_symmetric(ft_0, r_0.size());
    std::cout << ft_0 << "\n" << std::endl;

    Eigen::ArrayXf r_1(7);
    r_1 << 2.f, 1.f, 2.f, 3.f, 1.f, 1.f, 0.f;
    Eigen::ArrayX<std::complex<float>> ft_1 = FFT::r2c(r_1);
    FFT::make_hermitian_symmetric(ft_1, r_1.size());
    std::cout << ft_1 << "\n" << std::endl;

    Wave test_wave = Wave::from_samples(r_0, 2.0f);
    std::cout << test_wave << std::endl;

    Eigen::ArrayXf test_samples = Wave::sine(1.0f).samples(2.0f);
    Wave result = Wave::from_samples(test_samples);
    result.to_csv_samples(misc_output_dir / "From_Samples.csv", 2.0f, Wave::SAMPLE_RATE);
    EXPECT_TRUE(result.samples(2.0f).isApprox(test_samples, tolerance));
}

TEST_F(WaveTest, Pulse) {
    Wave result = Wave::pulse(2.0f, 0.66f, 30);
    result.to_csv_samples(misc_output_dir / "Pulse.csv", 1.0f, 2 * result.nyquist_rate());
}

TEST_F(WaveTest, Join) {
    Wave cos_wave = Wave::cosine(0.2f, 1.0f, 0.0f);
    Wave sin_wave = Wave::sine(0.4f, 1.0f, 0.0f);
    Wave result = cos_wave.join(sin_wave, 10.0f, 5.0f, 100);
    result.to_csv_samples(misc_output_dir / "Join.csv", 15.0f, 2 * result.nyquist_rate());
}

TEST_F(WaveTest, Neg_0) { compare_wave_result_with_truth_signal(-random_waves[0], "Neg_0.csv"); }
TEST_F(WaveTest, Neg_1) { compare_wave_result_with_truth_signal(-random_waves[1], "Neg_1.csv"); }
TEST_F(WaveTest, Add_0_1) { compare_wave_result_with_truth_signal(random_waves[0] + random_waves[1], "Add_0_1.csv"); }
TEST_F(WaveTest, Add_1_0) { compare_wave_result_with_truth_signal(random_waves[1] + random_waves[0], "Add_1_0.csv"); }
TEST_F(WaveTest, Sub_0_1) { compare_wave_result_with_truth_signal(random_waves[0] - random_waves[1], "Sub_0_1.csv"); }
TEST_F(WaveTest, Sub_1_0) { compare_wave_result_with_truth_signal(random_waves[1] - random_waves[0], "Sub_1_0.csv"); }
TEST_F(WaveTest, Mul_0_1) { compare_wave_result_with_truth_signal(random_waves[0] * random_waves[1], "Mul_0_1.csv"); }
TEST_F(WaveTest, Mul_1_0) { compare_wave_result_with_truth_signal(random_waves[1] * random_waves[0], "Mul_1_0.csv"); }
