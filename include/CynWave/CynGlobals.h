/* This document is not licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International license.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CYN_GLOBALS_H
#define CYN_GLOBALS_H

#include <type_traits>

namespace Cyn {

	// Intitially set to: 1e-3
	double DEFAULT_TOLERANCE = 1e-3;

	template <typename T>
	concept NumA = std::is_arithmetic_v<T>;

	template <typename T>
	concept NumF = std::is_floating_point_v<T>;

	template<typename T>
	concept NumI = std::is_integral_v<T>;

	template<typename T>
	concept NumUI = std::is_unsigned_v<T> && std::is_integral_v<T>;

	template<NumA T>
	inline constexpr T pi(long double factor = 1.0L) { return static_cast<T>(3.141592653589793L * factor); }

} // namespace Cyn

#endif // CYN_GLOBALS_H