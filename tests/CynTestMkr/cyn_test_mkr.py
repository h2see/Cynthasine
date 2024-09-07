"""
This document is not licensed under the Creative Commons
Attribution-NonCommercial-ShareAlike 4.0 International license.
Instead, it is licensed under the MIT License as specified below.
A copy of the MIT License can also be found in the LICENSE-MIT file,
if included in the distribution.


MIT License

Copyright (c) 2024 https://github.com/h2see

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import tomllib
from pathlib import Path
import numpy as np

# Set up paths and configuration
this_folder = Path(__file__).parent
test_data_folder = this_folder.parent / "test_data"
random_waves_folder = test_data_folder / "random_waves"
truth_signals_folder = test_data_folder / "truth_signals"

assert random_waves_folder.exists(), "test_data/random_waves folder does not exist."
assert truth_signals_folder.exists(), "test_data/truth_signals folder does not exist."

with open(this_folder / "config.toml", "rb") as file:
    config = tomllib.load(file)

rng = np.random.default_rng(seed=config["seed"])


def generate_random_values(wave_key, param_key):
    """Generate random values based on configuration settings."""
    param_config = config["wave"][wave_key][param_key]
    if param_config["use_int"]:
        return rng.integers(
            low=int(param_config["low"]),
            high=int(param_config["high"]),
            size=config["wave"][wave_key]["num"],
        )
    else:
        return rng.uniform(
            low=float(param_config["low"]),
            high=float(param_config["high"]),
            size=config["wave"][wave_key]["num"],
        )


def generate_wave_data(wave_key):
    """Generate frequency, amplitude, and phase data for a given wave key."""
    freq = generate_random_values(wave_key, "freq")
    amp = generate_random_values(wave_key, "amp")
    phase = generate_random_values(wave_key, "phase")
    return np.column_stack([freq, amp, phase])


def generate_all_waves():
    """Generate all waves as specified in the configuration."""
    return {int(wave_key): generate_wave_data(wave_key) for wave_key in config["wave"]}


def save_all_waves_to_csv(waves: dict):
    for wave_key, random_wave in waves.items():
        fpath = random_waves_folder / f"{wave_key}.csv"
        with open(fpath, mode="w") as f:
            f.write("freq, amp, phase\n")
            for row in random_wave:
                f.write(",".join(map(str, row)) + "\n")


def sample_waveform(arr: np.ndarray, duration: float, sample_rate: float) -> tuple[np.ndarray, np.ndarray]:
    """Generate samples from wave data over a given duration and sample rate."""
    num_samples = int(np.round(duration * sample_rate))
    timestamps = np.linspace(0, duration, num_samples)
    sampled_signal = np.sum(
        arr[:, 1][:, np.newaxis]
        * np.sin(2 * np.pi * arr[:, 0][:, np.newaxis] * timestamps - arr[:, 2][:, np.newaxis]),
        axis=0,
    )
    return timestamps, sampled_signal


def calculate_duration(w0: np.ndarray, w1: np.ndarray) -> float:
    """Calculate the duration based on the minimum frequency of two waves."""
    min_frequency = min(np.abs(w0[:, 0]).min(), np.abs(w1[:, 0]).min())
    return 2 / min_frequency


def calculate_sample_rate(w0: np.ndarray, w1: np.ndarray) -> float:
    """Calculate the sample rate based on the maximum frequency of two waves."""
    max_frequency = max(np.abs(w0[:, 0]).max(), np.abs(w1[:, 0]).max())
    return 2 * max_frequency


def perform_operations(w0: np.ndarray, w1: np.ndarray, duration: float, sample_rate: float):
    """Perform signal operations (negation, addition, subtraction, multiplication) on waveforms."""
    t, s0 = sample_waveform(w0, duration, sample_rate)
    _, s1 = sample_waveform(w1, duration, sample_rate)

    return {
        "negated_s0": -s0,
        "negated_s1": -s1,
        "added_s0_s1": s0 + s1,
        "added_s1_s0": s1 + s0,
        "subed_s0_s1": s0 - s1,
        "subed_s1_s0": s1 - s0,
        "muled_s0_s1": s0 * s1,
        "muled_s1_s0": s1 * s0,
        "time": t,
    }


def save_sampled_wave_data_to_csv(times: np.ndarray, signal: np.ndarray, filename: str):
    """Save the sampled wave data to a CSV file."""
    fpath = truth_signals_folder / filename
    assert fpath.suffix == ".csv", "File must have a .csv extension"

    if len(times) != len(signal):
        raise ValueError("The length of 'times' and 'signal' must be the same.")

    with open(fpath, mode="w") as f:
        f.write("Time, Signal\n")
        for t, s in zip(times, signal):
            f.write(f"{t}, {s}\n")


# Main execution
waves = generate_all_waves()
save_all_waves_to_csv(waves)

duration = calculate_duration(waves[0], waves[1])
sample_rate = calculate_sample_rate(waves[0], waves[1])
wave_operations = perform_operations(waves[0], waves[1], duration, sample_rate)

# Save results to CSV files
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["negated_s0"], "Neg_0.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["negated_s1"], "Neg_1.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["added_s0_s1"], "Add_0_1.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["added_s1_s0"], "Add_1_0.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["subed_s0_s1"], "Sub_0_1.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["subed_s1_s0"], "Sub_1_0.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["muled_s0_s1"], "Mul_0_1.csv")
save_sampled_wave_data_to_csv(wave_operations["time"], wave_operations["muled_s1_s0"], "Mul_1_0.csv")

print("\ntruth_signals creation complete.\n")
