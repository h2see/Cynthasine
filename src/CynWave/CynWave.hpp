/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#ifndef CYN_WAVE_HPP
#define CYN_WAVE_HPP

#include "CynEigenUtils.h"

namespace Cyn {

	template<NumF WaveT>
	class WaveArray : public Eigen::Array<WaveT, Eigen::Dynamic, 3> {
	public:

		// Inherit Class Constructors

		using Eigen::Array<WaveT, Eigen::Dynamic, 3>::Array;


		// Class Variables

		inline static WaveT SAMPLE_RATE = static_cast<WaveT>(44100);
		inline static WaveT TOLERANCE = static_cast<WaveT>(DEFAULT_TOLERANCE);


		// Accessors

		inline auto freq() {
			return this->col(0);
		}
		inline const auto freq() const {
			return this->col(0);
		}
		inline auto amp() {
			return this->col(1);
		}
		inline const auto amp() const {
			return this->col(1);
		}
		inline auto phase() {
			return this->col(2);
		}
		inline const auto phase() const {
			return this->col(2);
		}
		inline auto wave(Eigen::Index idx) {
			return this->row(idx);
		}
		inline const auto wave(Eigen::Index idx) const {
			return this->row(idx);
		}
		inline auto waves(Eigen::Index start_idx, Eigen::Index num_waves) {
			return this->block(start_idx, 0, num_waves, 3);
		}
		inline const auto waves(Eigen::Index start_idx, Eigen::Index num_waves) const {
			return this->block(start_idx, 0, num_waves, 3);
		}

		// Min Max Methods

		inline WaveT min_freq() const {
			return this->freq().minCoeff();
		}
		inline WaveT min_amp() const {
			return this->amp().minCoeff();
		}
		inline WaveT min_phase() const {
			return this->phase().minCoeff();
		}
		inline WaveT max_freq() const {
			return this->freq().maxCoeff();
		}
		inline WaveT max_amp() const {
			return this->amp().maxCoeff();
		}
		inline WaveT max_phase() const {
			return this->phase().maxCoeff();
		}

		// Convenience Methods

		inline WaveT nyquist_rate() const {
			return this->max_freq() * 2;
		}
		static inline WaveT nyquist_freq(std::optional<WaveT> sample_rate = std::nullopt) {
			return sample_rate.value_or(SAMPLE_RATE) / 2;
		}
		inline Eigen::Index num_waves() const {
			return this->rows();
		}

		// IO Methods

		inline void to_csv(const std::filesystem::path& filename) const {
			save_to_csv(filename, *this, std::vector<std::string>{"freq", "amp", "phase"});
		}

		inline static WaveArray<WaveT> from_csv(const std::filesystem::path& filename) {
			std::vector<std::string> header;
			auto csv_array = load_from_csv<WaveT>(filename, &header);
			if (header.size() != 3) {
				throw std::runtime_error("The CSV file must have a header with three columns.");
			}
			if (header[0] != "freq" || header[1] != "amp" || header[2] != "phase") {
				throw std::runtime_error("The CSV file must contain freq, amp, then phase as the column headers.");
			}
			return std::move(csv_array);
		}

		// Class Factory Methods

		inline static WaveArray<WaveT> zero() {
			WaveArray<WaveT> result(1, 3);
			result << static_cast<WaveT>(0), static_cast<WaveT>(0), static_cast<WaveT>(0);
			return result;
		}

		inline static WaveArray<WaveT> one() {
			WaveArray<WaveT> result(1, 3);
			result << static_cast<WaveT>(0), static_cast<WaveT>(1), pi<WaveT>(1.5L);
			return result;
		}

		inline static WaveArray<WaveT> sine(WaveT frequency, WaveT amplitude = static_cast<WaveT>(1), WaveT phase = static_cast<WaveT>(0)) {
			WaveArray<WaveT> result(1, 3);
			result << frequency, amplitude, phase;
			return result;
		}

		inline static WaveArray<WaveT> cosine(WaveT frequency, WaveT amplitude = static_cast<WaveT>(1), WaveT phase = static_cast<WaveT>(0)) {
			WaveArray<WaveT> result(1, 3);
			result << frequency, amplitude, phase + pi<WaveT>(1.5L);
			return result;
		}

		static WaveArray<WaveT> square(WaveT frequency, Eigen::Index num_waves) {
			WaveArray<WaveT> result(num_waves, 3);
			auto n = Eigen::ArrayX<WaveT>::LinSpaced(num_waves, static_cast<WaveT>(1), static_cast<WaveT>(2 * num_waves - 1));
			result.freq() = n * frequency;
			result.amp() = (1 / pi<WaveT>(0.25L)) / n;
			result.phase().setZero();
			return result;
		}

		static WaveArray<WaveT> triangle(WaveT frequency, Eigen::Index num_waves) {
			WaveArray<WaveT> result(num_waves, 3);
			auto n = Eigen::ArrayX<WaveT>::LinSpaced(num_waves, static_cast<WaveT>(1), static_cast<WaveT>(2 * num_waves - 1));
			result.freq() = n * frequency;
			result.amp() = alternating_signs(num_waves, 1).cast<WaveT>() / (n.pow(2) * (pi<WaveT>(0.25L) * pi<WaveT>(0.5L)));
			result.phase().setZero();
			return result;
		}

		static WaveArray<WaveT> sawtooth(WaveT frequency, Eigen::Index num_waves) {
			WaveArray<WaveT> result(num_waves, 3);
			auto n = Eigen::ArrayX<WaveT>::LinSpaced(num_waves, static_cast<WaveT>(1), static_cast<WaveT>(num_waves));
			result.freq() = n * frequency;
			result.amp() = alternating_signs(num_waves, 1).cast<WaveT>() / (n * pi<WaveT>(0.5L));
			result.phase().setZero();
			return result;
		}

		static WaveArray<WaveT> pulse(WaveT frequency, WaveT duty_cycle, Eigen::Index num_waves) {
			WaveArray<WaveT> result(num_waves, 3);
			result.wave(0) << static_cast<WaveT>(0), duty_cycle * 2 - 1, pi<WaveT>(1.5L);
			Eigen::Index num_w = num_waves - 1;
			auto n = Eigen::ArrayX<WaveT>::LinSpaced(num_w, static_cast<WaveT>(1), static_cast<WaveT>(num_w));
			result.freq().tail(num_w) = n * frequency;
			result.amp().tail(num_w) = (n * (duty_cycle * pi<WaveT>())).sin() / (n * pi<WaveT>(0.25L));
			result.phase().tail(num_w).setConstant(pi<WaveT>(1.5L));
			return result;
		}

		// Calculus

		inline WaveArray<WaveT> derivative(int n = 1) const {
			WaveArray<WaveT> result = *this;
			result.amp() *= (result.freq() * pi<WaveT>(2.0L)).pow(n);
			result.phase() -= n * pi<WaveT>(0.5L);
			return result;
		}
		inline WaveArray<WaveT> integral(int n = 1) const {
			n *= -1;
			return this->derivative(n);
		}

		// Sorting

		WaveArray<WaveT> sort(Eigen::Index sort_by_col_idx = 0, bool ascending = true, std::optional<WaveT> tolerance = std::nullopt) const {
			if (sort_by_col_idx < 0 || sort_by_col_idx > 2) {
				throw std::out_of_range("Invalid column index.");
			}
			Eigen::Index num_w = this->num_waves();
			if (num_w == 0 || num_w == 1) { return *this; }
			std::vector<Eigen::Index> idx(num_w);
			WaveT tol = tolerance.value_or(TOLERANCE);
			std::iota(idx.begin(), idx.end(), 0);
			std::sort(idx.begin(), idx.end(), [&](Eigen::Index a, Eigen::Index b) {
					for (int offset = 0; offset < 3; ++offset) {
						Eigen::Index col_idx = (sort_by_col_idx + offset) % 3;
						const WaveT& a_val = this->operator()(a, col_idx);
						const WaveT& b_val = this->operator()(b, col_idx);
						if (!isclose(a_val, b_val, tol)) {
							return ascending ? a_val < b_val : a_val > b_val;
						}
					}
					return false;
				}
			);
			WaveArray<WaveT> result(num_w, 3);
			for (Eigen::Index i = 0; i < num_w; ++i) {
				result.wave(i) = this->wave(idx[i]);
			}
			return result;
		}
		inline WaveArray<WaveT> sort_by_freq(bool ascending = true, std::optional<WaveT> tolerance = std::nullopt) const {
			return sort(0, ascending, tolerance);
		}
		inline WaveArray<WaveT> sort_by_amp(bool ascending = true, std::optional<WaveT> tolerance = std::nullopt) const {
			return sort(1, ascending, tolerance);
		}
		inline WaveArray<WaveT> sort_by_phase(bool ascending = true, std::optional<WaveT> tolerance = std::nullopt) const {
			return sort(2, ascending, tolerance);
		}

		// Filtering

		WaveArray<WaveT> filter(const Eigen::ArrayXb& keep_mask) const {
			Eigen::Index keep_num = keep_mask.count();
			WaveArray<WaveT> result(keep_num, 3);
			if (keep_num == 0) { return result; }
			Eigen::Index mask_size = keep_mask.size();
			if (mask_size != this->num_waves()) {
				throw std::invalid_argument("keep_mask size must be equivalent to this->num_waves().");
			}
			Eigen::Index result_index = 0;
			for (Eigen::Index i = 0; i < mask_size; ++i) {
				if (keep_mask[i]) {
					result.wave(result_index++) = this->wave(i);
				}
			}
			return result;
		}

		inline WaveArray<WaveT> remove_zero(std::optional<WaveT> tolerance = std::nullopt) const {
			return this->filter(this->amp().abs() > tolerance.value_or(TOLERANCE));
		}

		inline WaveArray<WaveT> remove_bias(std::optional<WaveT> tolerance = std::nullopt) const {
			return this->filter(this->freq().abs() > tolerance.value_or(TOLERANCE));
		}

		// Parameter Standardization

		void standardize_params_inplace(std::optional<WaveT> tolerance = std::nullopt) {
			WaveT tol = tolerance.value_or(TOLERANCE);
			Eigen::Index num_w = this->num_waves();
			for (Eigen::Index i = 0; i < num_w; ++i) {
				WaveT& this_freq = this->operator()(i, 0);
				WaveT& this_amp = this->operator()(i, 1);
				WaveT& this_phase = this->operator()(i, 2);
				if (iszero(this_amp, tol)) {
					this_freq = 0;
					this_amp = 0;
					this_phase = 0;
					continue;
				}
				else if (this_amp < 0) {
					this_amp = -this_amp;
					this_phase = pi<WaveT>() + this_phase;
				}
				if (iszero(this_freq, tol)) {
					this_freq = 0;
				}
				else if (this_freq < 0) {
					this_freq = -this_freq;
					this_phase = pi<WaveT>() - this_phase;
				}
				this_phase = posmod(this_phase, pi<WaveT>(2.0L));
				if (iszero(this_phase, tol) || isclose(this_phase, pi<WaveT>(2.0L), tol)) {
					this_phase = 0;
				}
			}
		}

		inline WaveArray<WaveT> standardize_params(std::optional<WaveT> tolerance = std::nullopt) const {
			WaveArray<WaveT> result = *this;
			result.standardize_params_inplace(tolerance);
			return result;
		}

		// Interference

		WaveArray<WaveT> interfere(std::optional<WaveT> tolerance = std::nullopt) const {
			Eigen::Index this_num_w = this->num_waves();
			if (this_num_w == 0 || this_num_w == 1) { return *this; }
			WaveT tol = tolerance.value_or(TOLERANCE);
			WaveArray<WaveT> result = this->remove_zero(tol);
			Eigen::Index num_w = result.num_waves();
			if (num_w == 0 || num_w == 1) { return result; }
			result.standardize_params_inplace(tol);
			auto freq_arr = result.freq().replicate(1, num_w);
			auto phase_arr = result.phase().replicate(1, num_w);
			Eigen::MatrixXb same_freq(num_w, num_w), constructive(num_w, num_w), destructive(num_w, num_w);
			constructive.diagonal().setOnes();
			constructive.triangularView<Eigen::StrictlyUpper>().setZero();
			destructive.triangularView<Eigen::Upper>().setZero();
			same_freq.triangularView<Eigen::StrictlyLower>() = freq_arr.isclose(freq_arr.transpose(), tol).matrix();
			constructive.triangularView<Eigen::StrictlyLower>() = phase_arr.isclose(phase_arr.transpose(), tol).matrix() && same_freq;
			destructive.triangularView<Eigen::StrictlyLower>() = phase_arr.isclose((phase_arr.transpose() - pi<WaveT>()).posmod(pi<WaveT>(2.0L)), tol).matrix() && same_freq;
			result.amp() = (constructive.cast<WaveT>().array().colwise() * result.amp()).colwise().sum() - (destructive.cast<WaveT>().array().colwise() * result.amp()).colwise().sum();
			constructive.diagonal().setZero();
			return result.filter(!((constructive || destructive).rowwise().any().array() || result.amp().iszero(tol)));
		}

		// WaveArray Standardization

		inline WaveArray<WaveT> standardize(std::optional<WaveT> tolerance = std::nullopt) const {
			WaveArray<WaveT> interfered = this->interfere(tolerance);
			interfered.standardize_params_inplace(tolerance);
			return interfered.sort(0, true, tolerance);
		}

		// Sampling

		inline static Eigen::ArrayX<WaveT> generate_timestamps(WaveT duration, std::optional<WaveT> sample_rate = std::nullopt) {
			Eigen::Index num_samples = static_cast<Eigen::Index>(std::round(duration * sample_rate.value_or(SAMPLE_RATE)));
			return Eigen::ArrayX<WaveT>::LinSpaced(num_samples, 0, duration);
		}

		inline WaveT sample(WaveT timestamp) const {
			Eigen::Index num_w = this->num_waves();
			if (num_w == 0) { return static_cast<WaveT>(0); }
			return (this->amp() * (this->freq() * (timestamp * pi<WaveT>(2.0L)) - this->phase()).sin()).sum();
		}

		Eigen::ArrayX<WaveT> samples(const Eigen::ArrayX<WaveT>& timestamps) const {
			Eigen::Index num_w = this->num_waves();
			if (num_w == 0) { return Eigen::ArrayX<WaveT>::Zero(timestamps.size()); }
			Eigen::ArrayX<WaveT> timestamps_scaled = timestamps * pi<WaveT>(2.0L);
			Eigen::ArrayX<WaveT> result = (timestamps_scaled * this->operator()(0, 0) - this->operator()(0, 2)).sin() * this->operator()(0, 1);
			for (Eigen::Index i = 1; i < num_w; ++i) {
				result += (timestamps_scaled * this->operator()(i, 0) - this->operator()(i, 2)).sin() * this->operator()(i, 1);
			}
			return result;
		}

		inline Eigen::ArrayX<WaveT> samples(WaveT duration, std::optional<WaveT> sample_rate = std::nullopt, Eigen::ArrayX<WaveT>* generated_timestamps = nullptr) const {
			Eigen::ArrayX<WaveT> timestamps = this->generate_timestamps(duration, sample_rate);
			if (generated_timestamps != nullptr) { *generated_timestamps = timestamps; }
			return this->samples(timestamps);
		}

		inline void to_csv_samples(const std::filesystem::path& filename, WaveT duration, std::optional<WaveT> sample_rate = std::nullopt, std::string timestamps_title = "Time", std::string samples_title = "Signal") const {
			Eigen::ArrayX<WaveT> time;
			Eigen::ArrayX<WaveT> signal = samples(duration, sample_rate.value_or(this->nyquist_rate()), &time);
			save_to_csv(filename, time, signal, timestamps_title, samples_title);
		}

		// Fourier

		static WaveArray<WaveT> from_samples(const Eigen::ArrayX<WaveT>& samples, std::optional<WaveT> sample_rate = std::nullopt, std::optional<WaveT> tolerance = std::nullopt, const size_t& num_threads = 1) {
			Eigen::Index samples_size = samples.size();
			Eigen::ArrayX<std::complex<WaveT>> ft = FFT::r2c(samples, false, num_threads);
			Eigen::Index half_size = ft.size();
			Eigen::Index conjugate_size = samples_size - half_size;
			std::complex<WaveT> i(static_cast<WaveT>(0), static_cast<WaveT>(1));
			std::complex<WaveT> cn;
			std::complex<WaveT> cng;
			WaveT an;
			WaveT bn;
			WaveT frequency;

			Eigen::Index n2;
			WaveArray<WaveT> result(samples_size, 3);
			result.row(0) << static_cast<WaveT>(0), ft(0).real() / static_cast<WaveT>(samples_size), pi<WaveT>(1.5L);
			for (Eigen::Index n = 1; n <= conjugate_size; ++n) {
				n2 = n * 2;
				cn = ft(n);
				cng = std::conj(cn);
				an = std::real(cn + cng) / static_cast<WaveT>(samples_size);
				bn = std::real(i * (cn - cng)) / static_cast<WaveT>(samples_size);
				frequency = (static_cast<WaveT>(n2) / static_cast<WaveT>(samples_size)) * nyquist_freq(sample_rate);
				result.row(n2 - 1) << frequency, an, pi<WaveT>(1.5L);
				result.row(n2) << frequency, bn, static_cast<WaveT>(0);
			}
			if (samples_size % 2 == 0) {
				result.row(samples_size - 1) << nyquist_freq(sample_rate), ft(half_size - 1).real() / static_cast<WaveT>(samples_size), pi<WaveT>(1.5L);
			}
			if (tolerance.has_value()) {
				if (tolerance.value() < -0.5) {
					return result;
				}
			}
			return result.remove_zero(tolerance);
		}

		inline void shift_inplace(WaveT phase_shift) {
			this->phase() += this->freq() * (phase_shift * pi<WaveT>(2.0L));
		}

		inline void shift_inplace(const Eigen::ArrayX<WaveT>& phase_shift) {
			if (phase_shift.size() != this->num_waves()) { throw std::invalid_argument("Phase shift array must be num_waves() in size."); }
			this->phase() += this->freq() * (phase_shift * pi<WaveT>(2.0L));
		}

		inline WaveArray<WaveT> shift(WaveT phase_shift) const {
			WaveArray<WaveT> result = *this;
			result.shift_inplace(phase_shift);
			return result;
		}

		inline WaveArray<WaveT> shift(const Eigen::ArrayX<WaveT>& phase_shift) const {
			WaveArray<WaveT> result = *this;
			result.shift_inplace(phase_shift);
			return result;
		}

		WaveArray<WaveT> join(const WaveArray<WaveT>& wave_b, WaveT wave_a_duration, WaveT wave_b_duration, Eigen::Index num_waves) const {
			WaveT frequency = static_cast<WaveT>(1) / (wave_a_duration + wave_b_duration);
			WaveArray<WaveT> pulse_modulator = WaveArray<WaveT>::pulse(frequency, wave_a_duration * frequency, num_waves);
			pulse_modulator /= 2;
			pulse_modulator += 0.5;
			pulse_modulator.shift_inplace(wave_a_duration / 2);
			WaveArray<WaveT> wave_a_contribution = *this * pulse_modulator;
			pulse_modulator *= -1;
			pulse_modulator += 1;
			WaveArray<WaveT> wave_b_contribution = wave_b * pulse_modulator;
			return wave_a_contribution + wave_b_contribution;
		}

		#ifdef CYN_WAVE_ARRAY_ADDON
		#include CYN_WAVE_ARRAY_ADDON
		#endif // CYN_WAVE_ARRAY_ADDON

	}; // class WaveArray

	// ========================================================================
	// Operations

	// WaveArray Equality
	template<NumF WaveT>
	inline bool operator==(const WaveArray<WaveT>& lhs, const WaveArray<WaveT>& rhs) {
		WaveArray<WaveT> lhs_standardized = lhs.standardize(lhs.TOLERANCE);
		WaveArray<WaveT> rhs_standardized = rhs.standardize(lhs.TOLERANCE);
		if (lhs_standardized.num_waves() != rhs_standardized.num_waves()) { return false; }
		return lhs_standardized.isApprox(rhs_standardized, lhs.TOLERANCE);
	}

	// Negation
	template<NumF WaveT>
	inline WaveArray<WaveT> operator-(WaveArray<WaveT> rhs) {
		rhs.amp() *= -1;
		return rhs;
	}

	// NumA Addition Assignment
	template<NumF WaveT, NumA T>
	WaveArray<WaveT>& operator+=(WaveArray<WaveT>& lhs, T rhs) {
		Eigen::Index num_w_lhs = lhs.num_waves();
		lhs.conservativeResize(num_w_lhs + 1, Eigen::NoChange);
		lhs.wave(num_w_lhs) << static_cast<WaveT>(0), static_cast<WaveT>(rhs), pi<WaveT>(1.5L);
		return lhs;
	}

	// WaveArray Addition Assignment
	template<NumF WaveT>
	WaveArray<WaveT>& operator+=(WaveArray<WaveT>& lhs, const WaveArray<WaveT>& rhs) {
		Eigen::Index num_w_lhs = lhs.num_waves();
		Eigen::Index num_w_rhs = rhs.num_waves();
		lhs.conservativeResize(num_w_lhs + num_w_rhs, Eigen::NoChange);
		lhs.waves(num_w_lhs, num_w_rhs) = rhs;
		return lhs;
	}

	// NumA Addition
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator+(WaveArray<WaveT> lhs, T rhs) {
		lhs += rhs;
		return lhs;
	}

	// NumA Addition Reflected
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator+(T lhs, WaveArray<WaveT> rhs) {
		rhs += lhs;
		return rhs;
	}

	// WaveArray Addition
	template<NumF WaveT>
	inline WaveArray<WaveT> operator+(WaveArray<WaveT> lhs, const WaveArray<WaveT>& rhs) {
		lhs += rhs;
		return lhs;
	}

	// NumA Subtraction Assignment
	template<NumF WaveT, NumA T>
	WaveArray<WaveT>& operator-=(WaveArray<WaveT>& lhs, T rhs) {
		Eigen::Index num_w_lhs = lhs.num_waves();
		lhs.conservativeResize(num_w_lhs + 1, Eigen::NoChange);
		lhs.wave(num_w_lhs) << static_cast<WaveT>(0), static_cast<WaveT>(-rhs), pi<WaveT>(1.5L);
		return lhs;
	}

	// WaveArray Subtraction Assignment
	template<NumF WaveT>
	WaveArray<WaveT>& operator-=(WaveArray<WaveT>& lhs, WaveArray<WaveT> rhs) {
		Eigen::Index num_w_lhs = lhs.num_waves();
		Eigen::Index num_w_rhs = rhs.num_waves();
		lhs.conservativeResize(num_w_lhs + num_w_rhs, Eigen::NoChange);
		rhs.amp() *= -1;
		lhs.waves(num_w_lhs, num_w_rhs) = rhs;
		return lhs;
	}

	// NumA Subtraction
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator-(WaveArray<WaveT> lhs, T rhs) {
		lhs -= rhs;
		return lhs;
	}

	// NumA Subtraction Reflected
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator-(T lhs, WaveArray<WaveT> rhs) {
		rhs.amp() *= -1;
		rhs += lhs;
		return rhs;
	}

	// WaveArray Subtraction
	template<NumF WaveT>
	inline WaveArray<WaveT> operator-(WaveArray<WaveT> lhs, const WaveArray<WaveT>& rhs) {
		lhs -= rhs;
		return lhs;
	}

	// WaveArray Multiplication
	template<NumF WaveT>
	WaveArray<WaveT> operator*(const WaveArray<WaveT>& lhs, const WaveArray<WaveT>& rhs) {
		Eigen::Array<WaveT, Eigen::Dynamic, Eigen::Dynamic> freq_lhs, freq_rhs, phase_lhs, phase_rhs;
		cartesian_product(lhs.freq(), rhs.freq(), freq_lhs, freq_rhs);
		cartesian_product(lhs.phase(), rhs.phase() + pi<WaveT>(0.5L), phase_lhs, phase_rhs);
		Eigen::Index half_num_waves = lhs.num_waves() * rhs.num_waves();
		WaveArray<WaveT> result(2 * half_num_waves, 3);
		result.block(0, 0, half_num_waves, 1) = (freq_lhs + freq_rhs).reshaped();
		result.block(0, 1, half_num_waves, 1) = (0.5 * lhs.amp().matrix() * rhs.amp().matrix().transpose()).reshaped();
		result.block(0, 2, half_num_waves, 1) = (phase_lhs + phase_rhs).reshaped();
		result.block(half_num_waves, 0, half_num_waves, 1) = (freq_lhs - freq_rhs).reshaped();
		result.block(half_num_waves, 1, half_num_waves, 1) = result.block(0, 1, half_num_waves, 1);
		result.block(half_num_waves, 2, half_num_waves, 1) = (phase_lhs - phase_rhs).reshaped();
		return result;
	}

	// WaveArray Multiplication Assignment
	template<NumF WaveT>
	inline WaveArray<WaveT>& operator*=(WaveArray<WaveT>& lhs, const WaveArray<WaveT>& rhs) {
		lhs = lhs * rhs;
		return lhs;
	}

	// NumA Multiplication Assignment
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT>& operator*=(WaveArray<WaveT>& lhs, T rhs) {
		lhs.amp() *= static_cast<WaveT>(rhs);
		return lhs;
	}

	// NumA Multiplication
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator*(WaveArray<WaveT> lhs, T rhs) {
		lhs *= rhs;
		return lhs;
	}

	// NumA Multiplication Reflected
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator*(T lhs, WaveArray<WaveT> rhs) {
		rhs *= lhs;
		return rhs;
	}

	// NumA Division Assignment
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT>& operator/=(WaveArray<WaveT>& lhs, T rhs) {
		lhs.amp() /= static_cast<WaveT>(rhs);
		return lhs;
	}

	// NumA Division
	template<NumF WaveT, NumA T>
	inline WaveArray<WaveT> operator/(WaveArray<WaveT> lhs, T rhs) {
		lhs /= rhs;
		return lhs;
	}

	// NumI Power
	template<NumF WaveT, NumI T>
	WaveArray<WaveT> operator^(WaveArray<WaveT> lhs, T rhs) {
		throw std::runtime_error("Not Implemented.");
		return WaveArray<WaveT>{0.0f, 0.0f, 0.0f};
	}

	// NumI Power Assignment
	template<NumF WaveT, NumI T>
	inline WaveArray<WaveT>& operator^=(WaveArray<WaveT>& lhs, T rhs) {
		lhs = lhs ^ rhs;
		return lhs;
	}

	using Wave = WaveArray<float>;
	using WaveF = WaveArray<float>;
	using WaveD = WaveArray<double>;
	using WaveL = WaveArray<long double>;

} // namespace Cyn

#endif // CYN_WAVE_HPP
