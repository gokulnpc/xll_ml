// tmx_valuation.h - present value, duration, convexity, yield, oas
#pragma once
#include <cmath>
#include "fms_curve.h"
#include "fms_instrument.h"
#include "fms_root1d.h"
namespace fms::value {
	// Convert between continuous rate and compounded yield using (1 + y/n)^n = e^r
	template<class X>
	inline X continuous_rate(X y, unsigned n)
	{
		return X(std::log(std::pow(1 + y / n, n)));
	}
	template<class X>
	inline X compound_yield(X r, unsigned n)
	{
		return X(n * std::expm1(r / n));
	}
	// Present value at t of a zero coupon bond with cash flow c at time u.
	template<class U, class C, class T, class F>
	constexpr C present(const instrument::base<U, C>& uc, const curve::base<T, F>& f)
	{
		C pv = 0;
		const U* u = uc.time();
		const C* c = uc.cash();
		for (size_t i = 0; i < uc.size(); ++i) {
			pv += c[i] * f.discount(u[i]);
		}
		return pv;
	}
	// Derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr auto duration(const instrument::base<U, C>& i, const curve::base<T, F>& f)
	{
		C dur = 0;
		const U* u = i.time();
		const C* c = i.cash();
		for (size_t j = 0; j < i.size(); ++j) {
			dur += -u[j] * c[j] * f.discount(u[j]);
		}
		return dur;
	}
	// Duration divided by present value.
	template<class U, class C, class T, class F>
	constexpr auto macaulay_duration(const instrument::base<U, C>& i, const curve::base<T, F>& f)
	{
		return duration(i, f) / present(i, f);
	}
	// Second derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr auto convexity(const instrument::base<U, C>& i, const curve::base<T, F>& f)
	{
		C cvx = 0;
		const U* u = i.time();
		const C* c = i.cash();
		for (size_t j = 0; j < i.size(); ++j) {
			cvx += u[j] * u[j] * c[j] * f.discount(u[j]);
		}
		return cvx;
	}
	// Price of the instrument at constant yield y.
	template<class U, class C>
	inline C price(const instrument::base<U, C>& i, C y)
	{
		return present(i, curve::constant<U, C>(y));
	}
	// Constant yield matching price p.
	template<class U, class C>
	inline auto yield(const instrument::base<U, C>& i, C p = 0,
		C y0 = 0.01, C tol = math::sqrt_epsilon<C>, int iter = 100)
	{
		const auto pv = [&i, p](C y_) { return present(i, curve::constant<U, C>(y_)) - p; };
		return root1d::secant(y0, y0 + 0.1, tol, iter).solve(pv);
	}
	// Option adjusted spread for which the present value of the instrument equals price.
	template<class U, class C, class T, class F>
	inline auto oas(const instrument::base<U, C>& i, const curve::base<T, F>& f, F p,
		F s0 = 0, F tol = math::sqrt_epsilon<F>, int iter = 100)
	{
		const auto pv = [p, &i, &f](F s_) { return present(i, f + curve::constant<T, F>(s_)) - p; };
		return root1d::secant(s0, s0 + .01, tol, iter).solve(pv);
	}
} // namespace fms::value