/* This document is not licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International license.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*
* Includes the <Eigen/Dense> header with the "CynEigenAddon.hpp" ArrayBase plugin by default.
* 
* Additionally Includes:
* 
*	- Eigen Type Aliases:
*		* VectorXb = Matrix<bool, Dynamic, 1>
*		* RowVectorXb = Matrix<bool, 1, Dynamic>
*		* MatrixXb = Matrix<bool, Dynamic, Dynamic>
*		* ArrayXb = Array<bool, Dynamic, 1>
*		* ArrayXXb = Array<bool, Dynamic, Dynamic>
*
*	- Project Headers:
*		* "CynUtils.h"
*		* "CynGlobals.h"
* 
*	- STL Headers:
*       * <algorithm>
*       * <cmath>
*       * <numeric>
*       * <optional>
*       * <stdexcept>
*       * <type_traits>
*/

#ifndef CYN_EIGEN_H
#define CYN_EIGEN_H

#include "CynUtils.h"

#define EIGEN_MPL2_ONLY

#ifndef EIGEN_ARRAYBASE_PLUGIN
#define EIGEN_ARRAYBASE_PLUGIN "CynEigenAddon.hpp"
#endif // EIGEN_ARRAYBASE_PLUGIN

#include <Eigen/Dense>

namespace Eigen {

	using VectorXb = Matrix<bool, Dynamic, 1>;
	using RowVectorXb = Matrix<bool, 1, Dynamic>;
	using MatrixXb = Matrix<bool, Dynamic, Dynamic>;
	using ArrayXb = Array<bool, Dynamic, 1>;
	using ArrayXXb = Array<bool, Dynamic, Dynamic>;

} // namespace Eigen

#endif // CYN_EIGEN_H
