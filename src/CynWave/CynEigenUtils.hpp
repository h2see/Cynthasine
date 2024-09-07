/*
 * Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
 * Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
 * copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
 */

#ifndef CYN_EIGEN_UTILS_HPP
#define CYN_EIGEN_UTILS_HPP

#include "CynEigen.h"

#include "pocketfft_hdronly.h"

#include <complex>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace pfft = pocketfft;

namespace Cyn {

	namespace impl {

		template <typename Derived>
		auto r2c(const Eigen::ArrayBase<Derived>& ArrayX_NumF, bool do_inverse = false, const size_t& num_threads = 1) {
			using T = typename Eigen::ArrayBase<Derived>::Scalar;
			pfft::shape_t axes{ static_cast<size_t>(0) };
			pfft::shape_t shape_in{ static_cast<size_t>(ArrayX_NumF.size())};
			pfft::stride_t stride_in{ static_cast<ptrdiff_t>(sizeof(T)) };
			pfft::stride_t stride_out{ static_cast<ptrdiff_t>(sizeof(std::complex<T>)) };
			Eigen::ArrayX<std::complex<T>> result(ArrayX_NumF.size() / 2 + 1);
			T scaling_factor = do_inverse ? static_cast<T>(1.0 / ArrayX_NumF.size()) : static_cast<T>(1);
			pfft::r2c(shape_in, stride_in, stride_out, axes, !do_inverse, ArrayX_NumF.derived().data(), result.data(), scaling_factor, num_threads);
			return std::move(result);
		}

		template <typename Derived>
		void make_hermitian_symmetric(const Eigen::ArrayBase<Derived>& ArrayX_NumC_inplace, const Eigen::Index& new_size) {
			Eigen::ArrayBase<Derived>& ArrayX_NumC_inplace_ = const_cast<Eigen::ArrayBase<Derived>&>(ArrayX_NumC_inplace);
			Eigen::Index reverse_conjugate_size = new_size - ArrayX_NumC_inplace_.size();
			ArrayX_NumC_inplace_.derived().conservativeResize(new_size);
			ArrayX_NumC_inplace_.tail(reverse_conjugate_size) = ArrayX_NumC_inplace_.segment(1, reverse_conjugate_size).reverse().conjugate();
		}

		template <typename Derived>
		auto c2c(const Eigen::ArrayBase<Derived>& ArrayX_NumC, bool do_inverse = false, const size_t& num_threads = 1) {
			using T = typename Eigen::ArrayBase<Derived>::Scalar::value_type;
			pfft::shape_t axes{ static_cast<size_t>(0) };
			pfft::shape_t shape{ static_cast<size_t>(ArrayX_NumC.size()) };
			pfft::stride_t stride{ static_cast<ptrdiff_t>(sizeof(std::complex<T>)) };
			Eigen::ArrayX<std::complex<T>> result(ArrayX_NumC.size());
			T scaling_factor = do_inverse ? static_cast<T>(1.0 / ArrayX_NumC.size()) : static_cast<T>(1);
			pfft::c2c(shape, stride, stride, axes, !do_inverse, ArrayX_NumC.derived().data(), result.data(), scaling_factor, num_threads);
			return std::move(result);
		}

		// ========================================================================

		template <typename Derived>
		auto gcd(const Eigen::ArrayBase<Derived>& ArrayXX_NumUI) {
			using T = typename Eigen::ArrayBase<Derived>::Scalar;
			Eigen::Index num_elements = ArrayXX_NumUI.size();
			if (num_elements == 0) return T(0);
			T result = ArrayXX_NumUI[0];
			for (Eigen::Index i = 1; i < num_elements; ++i) {
				result = gcd(result, ArrayXX_NumUI[i]);
				if (result == 1) break;
			}
			return result;
		}

		template <typename Derived>
		auto lcm(const Eigen::ArrayBase<Derived>& ArrayXX_NumUI) {
			using T = typename Eigen::ArrayBase<Derived>::Scalar;
			Eigen::Index num_elements = ArrayXX_NumUI.size();
			if (num_elements == 0) return T(0);
			T result = ArrayXX_NumUI[0];
			for (Eigen::Index i = 1; i < num_elements; ++i) {
				if (result == 0 || ArrayXX_NumUI[i] == 0) return T(0);
				result = lcm(result, ArrayXX_NumUI[i]);
			}
			return result;
		}

		// ========================================================================

		auto alternating_signs(Eigen::Index size, int start_value = 1) {
			if (start_value != 1 && start_value != -1) {
				throw std::invalid_argument("start_value must be either 1 or -1");
			}
			return Eigen::ArrayXi::LinSpaced(size, (start_value + 1) / 2, static_cast<int>(size) + (start_value - 1) / 2).unaryExpr([](int x) { return x % 2 * 2 - 1; });
		}

		template <typename Derived, NumA T>
		auto scale(const Eigen::ArrayBase<Derived>& ArrayXX_NumA, T new_min, T new_max, std::optional<T> old_min = std::nullopt, std::optional<T> old_max = std::nullopt) {
			T min_val, max_val;
			if (old_min) {
				min_val = old_min.value();
			}
			else {
				min_val = ArrayXX_NumA.minCoeff();
			}
			if (old_max) {
				max_val = old_max.value();
			}
			else {
				max_val = ArrayXX_NumA.maxCoeff();
			}
			return (ArrayXX_NumA - min_val) / (max_val - min_val) * (new_max - new_min) + new_min;
		}

		// ========================================================================

		template <typename DerivedA, typename DerivedB, typename DerivedC, typename DerivedD>
		void cartesian_product(const Eigen::ArrayBase<DerivedA>& a_ArrayX_NumA, const Eigen::ArrayBase<DerivedB>& b_ArrayX_NumA, const Eigen::ArrayBase<DerivedC>& a_ArrayXX_NumA_out, const Eigen::ArrayBase<DerivedD>& b_ArrayXX_NumA_out) {
			Eigen::ArrayBase<DerivedC>& a_ArrayXX_NumA_out_ = const_cast<Eigen::ArrayBase<DerivedC>&>(a_ArrayXX_NumA_out);
			Eigen::ArrayBase<DerivedD>& b_ArrayXX_NumA_out_ = const_cast<Eigen::ArrayBase<DerivedD>&>(b_ArrayXX_NumA_out);
			Eigen::Index a_size = a_ArrayX_NumA.size();
			Eigen::Index b_size = b_ArrayX_NumA.size();
			a_ArrayXX_NumA_out_.derived().resize(a_size, b_size);
			b_ArrayXX_NumA_out_.derived().resize(a_size, b_size);
			a_ArrayXX_NumA_out_ = a_ArrayX_NumA.replicate(1, b_size);
			b_ArrayXX_NumA_out_ = b_ArrayX_NumA.replicate(1, a_size).transpose();
		}

		template <typename Derived>
		void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<Derived>& ArrayX_NumA, const std::string& title) {
			std::ofstream csv_file(filename);
			if (!csv_file.is_open()) {
				throw std::runtime_error("Failed to open file: " + filename.string());
			}
			csv_file << title << "\n";
			Eigen::Index a_size = ArrayX_NumA.size();
			for (Eigen::Index i = 0; i < a_size; ++i) {
				csv_file << ArrayX_NumA[i] << "\n";
			}
			csv_file.close();
		}

		template <typename DerivedA, typename DerivedB>
		void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<DerivedA>& a_ArrayX_NumA, const Eigen::ArrayBase<DerivedB>& b_ArrayX_NumA, const std::optional<std::string>& a_title = std::nullopt, const std::optional<std::string>& b_title = std::nullopt) {
			std::ofstream csv_file(filename);
			if (!csv_file.is_open()) {
				throw std::runtime_error("Failed to open file: " + filename.string());
			}
			Eigen::Index a_size = a_ArrayX_NumA.size();
			if (a_size != b_ArrayX_NumA.size()) {
				csv_file.close();
				throw std::invalid_argument("Input Arrays must be the same size.");
			}
			if (a_title) {
				if (b_title) {
					csv_file << a_title.value() << "," << b_title.value() << "\n";
				}
				else {
					csv_file.close();
					throw std::invalid_argument("Both a_title and b_title must be provided.");
				}
			}
			for (Eigen::Index i = 0; i < a_size; ++i) {
				csv_file << a_ArrayX_NumA[i] << "," << b_ArrayX_NumA[i] << "\n";
			}
			csv_file.close();
		}


		template <typename Derived>
		void save_to_csv(const std::filesystem::path& filename, const Eigen::ArrayBase<Derived>& ArrayXX_NumA, const std::optional<std::vector<std::string>>& header = std::nullopt) {
			// Ensure the array is not empty
			if (ArrayXX_NumA.rows() == 0 || ArrayXX_NumA.cols() == 0) {
				throw std::runtime_error("The array is empty and cannot be saved to CSV.");
			}

			std::ofstream file(filename);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file: " + filename.string());
			}

			// Write the header if provided
			if (header.has_value()) {
				if (header->size() != ArrayXX_NumA.cols()) {
					throw std::runtime_error("Header size does not match the number of columns in the array.");
				}

				for (size_t i = 0; i < header->size(); ++i) {
					file << (*header)[i];
					if (i < header->size() - 1) {
						file << ",";
					}
				}
				file << "\n";
			}

			// Write the array data
			for (Eigen::Index row = 0; row < ArrayXX_NumA.rows(); ++row) {
				for (Eigen::Index col = 0; col < ArrayXX_NumA.cols(); ++col) {
					file << ArrayXX_NumA(row, col);
					if (col < ArrayXX_NumA.cols() - 1) {
						file << ",";
					}
				}
				file << "\n";
			}

			file.close();
		}



		template<NumA T>
		auto load_from_csv(const std::filesystem::path& filename, std::vector<std::string>* header = nullptr, T empty_value = T{0}) {
			// Ensure T is either integral or floating-point
			static_assert(NumI<T> || NumF<T>, "Template parameter T must be either integral or floating-point type.");

			if (!std::filesystem::exists(filename)) {
				throw std::runtime_error("File not found: " + filename.string());
			}

			std::ifstream file(filename);
			if (!file.is_open()) {
				throw std::runtime_error("Could not open file: " + filename.string());
			}

			std::string line;
			Eigen::Index row_count = 0;
			Eigen::Index col_count = 0;

			// Define regex pattern to remove trailing whitespaces from commas
			std::regex pattern(",\\s+");

			// Read the first line to count columns
			if (std::getline(file, line)) {
				line = std::regex_replace(line, pattern, ",");
				std::istringstream headerStream(line);
				std::string column;
				while (std::getline(headerStream, column, ',')) {
					if (header != nullptr) {
						header->push_back(column);  // Store header names
					}
					++col_count;  // Count the number of columns
				}

				if (header == nullptr) {
					// If no header is provided, consider the first line as data
					++row_count;
				}
			}

			// First pass: Count the number of rows (excluding the header if provided)
			while (std::getline(file, line)) {
				if (!line.empty()) {
					++row_count;
				}
			}

			// Reset the read pointer to the beginning of the file
			file.clear();  // Clear the EOF flag
			file.seekg(0, std::ios::beg);

			// Skip the first line if header is provided
			if (header != nullptr) {
				std::getline(file, line);
			}

			// Pre-allocate Array
			Eigen::ArrayXX<T> result(row_count, col_count);
			Eigen::Index row = 0;

			// Second pass: Read data into the Array
			while (std::getline(file, line)) {
				if (!line.empty()) {
					line = std::regex_replace(line, pattern, ",");
					std::istringstream lineStream(line);
					std::string cell;
					Eigen::Index col = 0;
					while (std::getline(lineStream, cell, ',')) {
						if (cell.empty()) {
							result(row, col) = empty_value;
						}
						else {
							try {
								if constexpr (NumI<T>) {
									result(row, col) = static_cast<T>(std::stoi(cell));
								}
								else if constexpr (NumF<T>) {
									result(row, col) = static_cast<T>(std::stod(cell));
								}
							}
							catch (const std::invalid_argument&) {
								throw std::runtime_error("Non-numeric data encountered: " + cell);
							}
						}
						++col;
					}
					++row;
				}
			}

			return result;
		}


	} // namespace impl

} // namespace Cyn

#endif // CYN_EIGEN_UTILS_HPP