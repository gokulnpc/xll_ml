// fms_curve_bootstrap.h - Bootstrap a piecewise-flat curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <stdexcept>
#include <utility>
#include "fms_instrument.h"
#include "fms_curve_pwflat.h"
#include "fms_valuation.h"
#include "fms_math.h"

namespace fms::curve {

	// bootstrap1 - cash deposit
	// bootstrap2 - forward rate agreement
	
	// Bootstrap a single instrument given last time on curve and optional initial forward rate guess.
	// Return point on the curve repricing the instrument.
	template<class U, class C, class T = double, class F = double>
	inline std::pair<T, F> bootstrap0(const instrument::instrument<U, C>& i, const curve::base<T, F>& f,
		T _t, F _f = math::NaN<F>, F p = 0)
	{
		const auto uc = i.last(); // last instrument cash flow
		if (uc.first <= _t) {
			return { math::NaN<T>, math::NaN<F> };
		}

		// fix up initial guess
		if (std::isnan(_f)) {
			_f = f(_t); // last forward rate
		}
		if (std::isnan(_f)) {
			_f = 0.01;
		}

		const auto vp = [&i, &f, _t, p](F f_) { 
			return value::present(i, extrapolate(f, _t, f_)) - p; 
		};

		auto [f_, tol, n] = root1d::secant(_f, _f + 0.01).solve(vp);

		return { uc.first, f_ };
	}

	// Bootstrap a piecewise flat curve from instruments and prices.
	template<class U = double, class C = double, class P = double>
	inline curve::pwflat<U, P> bootstrap(std::span<instrument::instrument<U,C>*> is, std::span<P> ps,
		double _t = 0, double _f = 0.03)
		//requires std::convertible_to<I,const instrument::base<U,C>&>
	{
		// Validate inputs
		if (is.size() != ps.size()) {
			throw std::invalid_argument("bootstrap: instruments and prices must have the same size");
		}

		curve::pwflat<U, P> f;
		for (std::size_t i = 0; i < is.size(); ++i) {
			// Validate pointer is not null
			if (is[i] == nullptr) {
				throw std::invalid_argument("bootstrap: instrument pointer is null");
			}

			// FIX: Use different variable name to avoid shadowing parameter _f
			const auto [t_next, f_next] = bootstrap0(*(is[i]), f, _t, _f, ps[i]);

			// Check for failed bootstrap
			if (std::isnan(t_next) || std::isnan(f_next)) {
				throw std::runtime_error("bootstrap: failed to bootstrap instrument");
			}

			f.push_back(t_next, f_next);
			_t = t_next;
			_f = f_next;  // Update forward rate guess for next iteration
		}

		return f;
	}
#ifdef _DEBUG
	inline int bootstrap_test()
	{
		{
			constexpr curve::constant<> f;
			constexpr double r = 0.1;
			const auto zcb = instrument::zero_coupon_bond(1, math::exp_approx(r));
			const auto D = f.discount(1, 0., r);
			assert(D < 1);
			auto p = value::present(zcb, extrapolate(f, 0., r));
			assert(math::abs(p - 1) < math::sqrt_epsilon<>);
			auto d = value::duration(zcb, extrapolate(f, 0., r));
			assert(math::abs(d - -1) < math::sqrt_epsilon<>);
			auto [_t, _f] = curve::bootstrap0(zcb, f, 0., 0.2, 1.);
			assert(_t == 1);
			assert(math::abs(_f - r) <= math::sqrt_epsilon<double>);
		}

		return 0;
	}
#endif // _DEBUG

}