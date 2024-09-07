/* This document is not licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International license.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CYN_UTILS_H
#define CYN_UTILS_H

#include "CynUtils.hpp"

namespace Cyn {

	/**
	 * @brief Compute the sign of a number.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns -1 if x is negative, 1 if x is positive, and 0 if x is zero.
	 */
	template<NumA T>
	inline T sgn(T x) {
		return impl::sgn(x);
	}

	/**
	 * @brief Compute the Heaviside step function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns 0 if x is less than zero, 0.5 if x is zero, and 1 if x is greater than zero.
	 */
	template<NumA T>
	inline T step(T x) {
		return impl::step(x);
	}

	/**
	 * @brief Compute the rectangular function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns 1 for |x| < 0.5, 0.5 for |x| = 0.5, and 0 otherwise.
	 */
	template<NumA T>
	inline T rect(T x) {
		return impl::rect(x);
	}

	/**
	 * @brief Compute the triangular function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns 1 - |x| for |x| < 1, and 0 otherwise.
	 */
	template<NumA T>
	inline T tri(T x) {
		return impl::tri(x);
	}

	/**
	 * @brief Compute the Gaussian function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns the Gaussian exp(- x^2 * pi).
	 */
	template<NumA T>
	inline T gauss(T x) {
		return impl::gauss(x);
	}

	/**
	 * @brief Compute the Hann function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns cos(x * pi)^2 for |x| < 0.5, and 0 otherwise.
	 */
	template<NumA T>
	inline T hann(T x) {
		return impl::hann(x);
	}

	/**
	 * @brief Compute the normalized sinc function.
	 *
	 * @param x Input number of any numeric type.
	 * 
	 * @return T Returns 1 if x is zero, sin(x * pi)/(x * pi) otherwise.
	 */
	template<NumA T>
	inline T sinc(T x) {
		return impl::sinc(x);
	}

	// ========================================================================

	/**
	 * @brief Compute the greatest common divisor of two unsigned integers.
	 *
	 * @param a First unsigned integer.
	 * @param b Second unsigned integer.
	 * 
	 * @return T Returns the greatest common divisor of a and b.
	 */
	template<NumUI T>
	inline T gcd(T a, T b) {
		return impl::gcd(a, b);
	}

	/**
	 * @brief Compute the least common multiple of two unsigned integers.
	 *
	 * @param a First unsigned integer.
	 * @param b Second unsigned integer.
	 * 
	 * @return T Returns the least common multiple of a and b.
	 */
	template<NumUI T>
	inline T lcm(T a, T b) {
		return impl::lcm(a, b);
	}

	// ========================================================================

	/**
	 * @brief Determine if a number is effectively zero within an optional tolerance.
	 *
	 * @param a Input number of any numeric type.
	 * 
	 * @param tolerance Optional tolerance level for zero comparison (defaults to DEFAULT_TOLERANCE if not provided).
	 * 
	 * @return bool Returns true if |a| < tolerance, false otherwise.
	 */
	template<NumA T>
	inline bool iszero(T a, std::optional<double> tolerance = std::nullopt) {
		return impl::iszero(a, tolerance);
	}

	/**
	 * @brief Determine if two numbers are close to each other within an optional tolerance.
	 *
	 * @param a First number of any numeric type.
	 * @param b Second number of any numeric type.
	 * 
	 * @param tolerance Optional tolerance level for closeness comparison (defaults to DEFAULT_TOLERANCE if not provided).
	 * 
	 * @return bool Returns true if |a - b| < tolerance, false otherwise.
	 */
	template<NumA T1, NumA T2>
	inline bool isclose(T1 a, T2 b, std::optional<double> tolerance = std::nullopt) {
		return impl::isclose(a, b, tolerance);
	}

	/**
	 * @brief Compute the positive modulus of a over b.
	 * 
	 * @tparam T Floating point type of the numerator and denominator.
	 * 
	 * @param a Numerator of any floating point type.
	 * @param b Denominator of any floating point type, should not be zero.
	 * 
	 * @return T Returns the result of a % b, adjusted to be positive if the result of the modulus is negative.
	 */
	template<NumF T>
	inline T posmod(T a, T b) {
		return impl::posmod(a, b);
	}

} // namespace Cyn

#endif // CYN_UTILS_H