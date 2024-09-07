/* This document is not licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International license.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

auto sgn() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::sgn(x); }); }
auto step() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::step(x); }); }
auto rect() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::rect(x); }); }
auto tri() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::tri(x); }); }
auto gauss() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::gauss(x); }); }
auto hann() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::hann(x); }); }
auto sinc() const { return this->unaryExpr([]<Cyn::NumA T>(T x) { return Cyn::sinc(x); }); }
auto iszero(std::optional<double> tolerance = std::nullopt) const { return this->abs() < tolerance.value_or(Cyn::DEFAULT_TOLERANCE); }
template<Cyn::NumA T>
auto isclose(T other, std::optional<double> tolerance = std::nullopt) const { return this->operator-(other).abs() < tolerance.value_or(Cyn::DEFAULT_TOLERANCE); }
template<typename OtherDerived>
auto isclose(const ArrayBase<OtherDerived>& other, std::optional<double> tolerance = std::nullopt) const { return this->operator-(other).abs() < tolerance.value_or(Cyn::DEFAULT_TOLERANCE); }
template<Cyn::NumF T>
auto posmod(T other) const { return this->unaryExpr([&other](T x) { return Cyn::posmod(x, other); }); }
template<typename OtherDerived, Cyn::NumF T>
auto posmod(const ArrayBase<OtherDerived>& other) const { return this->binaryExpr(other, [](T x, T y) { return Cyn::posmod(x, y); }); }
template<Cyn::NumF T>
auto posmod_inv(T other) const { return this->unaryExpr([&other](T x) { return Cyn::posmod(other, x); }); }
