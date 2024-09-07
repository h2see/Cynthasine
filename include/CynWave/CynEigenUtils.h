/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#ifndef CYN_EIGEN_UTILS_H
#define CYN_EIGEN_UTILS_H

#include "CynEigenUtils.hpp"

namespace Cyn {

	namespace FFT {

		/**
		 * @brief Performs a 1D real-to-complex Fourier transform on an Eigen array.
		 *
		 * This function uses PocketFFT to perform a 1D real-to-complex Fourier transform on the input array.
		 * It can also perform the inverse transform if specified.
		 *
		 * @tparam Derived A template parameter derived from Eigen::ArrayBase.
		 *
		 * @param ArrayX_NumF The input Eigen array of real numbers.
		 * @param do_inverse If true, performs the inverse transform. Defaults to false.
		 * @param num_threads Number of threads to use for computation. Defaults to 1.
		 *
		 * @return An Eigen::ArrayX<std::complex<T>> containing the Fourier-transformed data.
		 */
		template <typename Derived>
		inline auto r2c(const Eigen::ArrayBase<Derived>& ArrayX_NumF, bool do_inverse = false, const size_t& num_threads = 1) {
			return impl::r2c(ArrayX_NumF, do_inverse, num_threads);
		}


		template <typename Derived>
		inline void make_hermitian_symmetric(const Eigen::ArrayBase<Derived>& ArrayX_NumC_inplace, const Eigen::Index& new_size) {
			impl::make_hermitian_symmetric(ArrayX_NumC_inplace, new_size);
		}


		template <typename Derived>
		inline auto c2c(const Eigen::ArrayBase<Derived>& ArrayX_NumC, bool do_inverse = false, const size_t& num_threads = 1) {
			return impl::c2c(ArrayX_NumC, do_inverse, num_threads);
		}

	} // namespace FFT

	/**
	 * @brief Computes the greatest common divisor (GCD) of elements in an Eigen array.
	 *
	 * This function iteratively computes the GCD of the array elements. If the array is empty,
	 * it returns zero. Computation stops early if GCD is 1, as further computation is redundant.
	 *
	 * @tparam Derived A template parameter that should be derived from Eigen::ArrayBase.
	 * 
	 * @param ArrayXX_NumUI The input array of unsigned integers.
	 * 
	 * @return The GCD of all elements in the array.
	 */
	template <typename Derived>
	inline auto gcd(const Eigen::ArrayBase<Derived>& ArrayXX_NumUI) {
		return impl::gcd(ArrayXX_NumUI);
	}

	/**
	 * @brief Computes the least common multiple (LCM) of elements in an Eigen array.
	 *
	 * This function iteratively computes the LCM of the array elements. If any element
	 * is zero or the array is empty, it returns zero.
	 *
	 * @tparam Derived A template parameter that should be derived from Eigen::ArrayBase.
	 * 
	 * @param ArrayXX_NumUI The input array of unsigned integers.
	 * 
	 * @return The LCM of all elements in the array.
	 */
	template <typename Derived>
	inline auto lcm(const Eigen::ArrayBase<Derived>& ArrayXX_NumUI) {
		return impl::lcm(ArrayXX_NumUI);
	}

	// ========================================================================

	/**
	 * @brief Generates an Eigen array with alternating signs starting from a specified value.
	 *
	 * This function creates an array where the signs of successive elements alternate between +1 and -1,
	 * starting from either +1 or -1 as specified by the start_value parameter.
	 *
	 * @param size The size of the output Eigen array.
	 * @param start_value The starting value for the first element, must be either +1 or -1.
	 * 
	 * @return An Eigen::ArrayXi of the specified size with alternating signs.
	 * 
	 * @throw std::invalid_argument If the start_value is neither 1 nor -1.
	 */
	inline auto alternating_signs(Eigen::Index size, int start_value = 1) {
		return impl::alternating_signs(size, start_value);
	}

	/**
	 * @brief Scales the elements of an Eigen array from one range to another specified range.
	 *
	 * This function linearly maps the values from an original range (specified or automatically determined)
	 * to a new specified range [new_min, new_max].
	 *
	 * @tparam Derived A template parameter that should be derived from Eigen::ArrayBase.
	 * @tparam T The scalar type of the array elements.
	 * 
	 * @param ArrayXX_NumA The input Eigen array.
	 * @param new_min The lower bound of the new range.
	 * @param new_max The upper bound of the new range.
	 * 
	 * @param old_min Optional lower bound of the current range. If not provided, the minimum value of the array is used.
	 * @param old_max Optional upper bound of the current range. If not provided, the maximum value of the array is used.
	 * 
	 * @return A scaled Eigen array.
	 */
	template <typename Derived, NumA T>
	inline auto scale(const Eigen::ArrayBase<Derived>& ArrayXX_NumA, T new_min, T new_max, std::optional<T> old_min = std::nullopt, std::optional<T> old_max = std::nullopt) {
		return impl::scale(ArrayXX_NumA, new_min, new_max, old_min, old_max);
	}

	// ========================================================================

	/**
	 * @brief Computes the Cartesian product of two Eigen arrays and outputs the result in two output arrays.
	 *
	 * This function computes the Cartesian product of two input arrays a_ArrayX_NumA and b_ArrayX_NumA,
	 * storing the repeated values of a_ArrayX_NumA in a_ArrayXX_NumA_out and the repeated values of
	 * b_ArrayX_NumA in b_ArrayXX_NumA_out respectively.
	 *
	 * @tparam DerivedA, DerivedB, DerivedC, DerivedD Template parameters that should be derived from Eigen::ArrayBase.
	 * 
	 * @param a_ArrayX_NumA The first input Eigen array.
	 * @param b_ArrayX_NumA The second input Eigen array.
	 * 
	 * @param a_ArrayXX_NumA_out The output Eigen array for repeated values from a_ArrayX_NumA.
	 * @param b_ArrayXX_NumA_out The output Eigen array for repeated values from b_ArrayX_NumA.
	 */
	template <typename DerivedA, typename DerivedB, typename DerivedC, typename DerivedD>
	inline void cartesian_product(const Eigen::ArrayBase<DerivedA>& a_ArrayX_NumA, const Eigen::ArrayBase<DerivedB>& b_ArrayX_NumA, const Eigen::ArrayBase<DerivedC>& a_ArrayXX_NumA_out, const Eigen::ArrayBase<DerivedD>& b_ArrayXX_NumA_out) {
		impl::cartesian_product(a_ArrayX_NumA, b_ArrayX_NumA, a_ArrayXX_NumA_out, b_ArrayXX_NumA_out);
	}

	/**
	 * @brief Saves a single 1D Eigen array to a CSV file.
	 *
	 * This function saves data from an Eigen array to a specified CSV file. A title must be provided
	 * to be used for the header of the CSV file.
	 *
	 * @tparam Derived The Eigen type of the array to be saved.
	 * @param filename The name of the file to which the data will be written.
	 * @param ArrayX_NumA The Eigen array containing the data to be saved.
	 * @param title The header of the CSV file.
	 * @throw std::runtime_error If the file cannot be opened.
	 */
	template <typename Derived>
	inline void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<Derived>& ArrayX_NumA, const std::string& title) {
		impl::save_to_csv(filename, ArrayX_NumA, title);
	}

	/**
	 * @brief Saves two 1D Eigen arrays to a CSV file with optional titles for each array.
	 *
	 * This function saves data from two Eigen arrays to a specified CSV file. Optionally, titles can be provided for each array,
	 * which will be used as headers in the CSV file. The arrays must be of the same size.
	 *
	 * @tparam DerivedA The Eigen type of the first array to be saved.
	 * @tparam DerivedB The Eigen type of the second array to be saved.
	 * @param filename The name of the file to which the data will be written.
	 * @param a_ArrayX_NumA The first Eigen array containing the data to be saved.
	 * @param b_ArrayX_NumA The second Eigen array containing the data to be saved.
	 * @param a_title Optional parameter for the title of the first array. If provided, b_title must also be provided.
	 * @param b_title Optional parameter for the title of the second array. If provided, a_title must also be provided.
	 * @throw std::runtime_error If the file cannot be opened.
	 * @throw std::invalid_argument If the arrays are not of the same size or if only one title is provided when both are required.
	 */
	template <typename DerivedA, typename DerivedB>
	inline void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<DerivedA>& a_ArrayX_NumA, const Eigen::ArrayBase<DerivedB>& b_ArrayX_NumA, const std::optional<std::string>& a_title = std::nullopt, const std::optional<std::string>& b_title = std::nullopt) {
		impl::save_to_csv(filename, a_ArrayX_NumA, b_ArrayX_NumA, a_title, b_title);
	}


	/**
	 * @brief Saves a 2D Eigen array to a CSV file with an optional header.
	 *
	 * This function writes the contents of an Eigen array to a specified CSV file.
	 * If a header is provided, it will be used as the first row in the file.
	 *
	 * @tparam Derived The Eigen type of the array to be saved.
	 * @param filename The name of the file to which the data will be written.
	 * @param ArrayXX_NumA The Eigen array containing the data to be saved.
	 * @param header Optional parameter for the column headers. If provided, the size must match the number of columns in the array.
	 *
	 * @throw std::runtime_error If the array is empty, if the file cannot be opened,
	 *                           or if the header size does not match the number of columns.
	 *
	 * @note If the file already exists, it will be overwritten.
	 */
	template <typename Derived>
	inline void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<Derived>& ArrayXX_NumA, const std::optional<std::vector<std::string>>& header = std::nullopt) {
		impl::save_to_csv(filename, ArrayXX_NumA, header);
	}

	/**
	 * @brief Loads data from a CSV file into an Eigen array, optionally storing the header and handling empty cells.
	 *
	 * This function reads data from a CSV file into an Eigen array. If a header vector is provided, the first row of the file
	 * is treated as the header and stored in the vector; otherwise, it is treated as data. Empty cells in the CSV are replaced
	 * with a specified value. The function ensures that the template parameter T is either an integral or floating-point type.
	 *
	 * @tparam T The scalar type of the array elements, must be an integral or floating-point type.
	 *
	 * @param filename The path to the CSV file.
	 * @param header Optional pointer to a vector of strings to store the header row. If nullptr, the first row is treated as data.
	 * @param empty_value The value to insert for empty cells in the CSV. Defaults to 0.
	 *
	 * @return An Eigen::ArrayXX<T> containing the data from the CSV file.
	 *
	 * @throw std::runtime_error If the file does not exist, cannot be opened, or contains non-numeric data.
	 * @throw std::invalid_argument If T is not an integral or floating-point type.
	 */
	template<NumA T>
	inline auto load_from_csv(const std::filesystem::path& filename, std::vector<std::string>* header = nullptr, T empty_value = T{ 0 }) {
		return impl::load_from_csv(filename, header, empty_value);
	}

} // namespace Cyn

#endif // CYN_EIGEN_UTILS_H