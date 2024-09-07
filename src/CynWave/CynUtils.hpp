/* This document is not licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International license.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CYN_UTILS_HPP
#define CYN_UTILS_HPP

#include "CynGlobals.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <optional>
#include <stdexcept>

namespace Cyn {

	namespace impl {

		template<NumA T>
		inline T sgn(T x) {
			T zero = T(0);
			if (x == zero) {
				return zero;
			}
			else if (x < zero) {
				return T(-1);
			}
			return T(1);
		}

		template<NumA T>
		inline T step(T x) {
			T zero = T(0);
			if (x == zero) {
				return T(0.5);
			}
			else if (x > zero) {
				return T(1);
			}
			return T(0);
		}

		template<NumA T>
		inline T rect(T x) {
			auto abs_x = std::abs(x);
			T pt5 = T(0.5);
			if (abs_x == pt5) {
				return pt5;
			}
			else if (abs_x < pt5) {
				return T(1);
			}
			return T(0);
		}

		template<NumA T>
		inline T tri(T x) {
			auto abs_x = std::abs(x);
			T one = T(1);
			if (abs_x < one) {
				return static_cast<T>(one - abs_x);
			}
			return T(0);
		}

		template<NumA T>
		inline T gauss(T x) {
			return static_cast<T>(std::exp(x * x * -pi<double>()));
		}

		template<NumA T>
		inline T hann(T x) {
			if (std::abs(x) < 0.5) {
				auto cos_x_pi = std::cos(x * pi<double>());
				return static_cast<T>(cos_x_pi * cos_x_pi);
			}
			return T(0);
		}

		template<NumA T>
		inline T sinc(T x) {
			if (x == T(0)) { return T(1); }
			auto x_pi = x * pi<double>();
			return static_cast<T>(std::sin(x_pi) / x_pi);
		}

		// ========================================================================

		template<NumUI T>
		inline T gcd(T a, T b) {
			return std::gcd(a, b);
		}

		template<NumUI T>
		inline T lcm(T a, T b) {
			if (a == 0 || b == 0) return 0;
			return a / gcd(a, b) * b;
		}

		// ========================================================================

		template<NumA T>
		inline bool iszero(T a, std::optional<double> tolerance = std::nullopt) {
			return std::abs(a) < tolerance.value_or(DEFAULT_TOLERANCE);
		}

		template<NumA T1, NumA T2>
		inline bool isclose(T1 a, T2 b, std::optional<double> tolerance = std::nullopt) {
			return std::abs(a - b) < tolerance.value_or(DEFAULT_TOLERANCE);
		}

		template<NumF T>
		inline T posmod(T a, T b) {
			T mod = std::fmod(a, b);
			if ((mod < 0 && b > 0) || (mod > 0 && b < 0)) {
				mod += b;
			}
			return mod;
		}

	} // namespace impl

} // namespace Cyn

#endif // CYN_UTILS_HPP