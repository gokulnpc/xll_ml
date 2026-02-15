// fms_option.h - Generalized option pricing model.
// F = f exp(s X - kappa(s)), kappa(s) = log E[exp(s X)]
// E[F] = f, Var(log(F)) = s^2 if E[X] = 0 and Var(X) = 1
// E[(k - F)^+] = E[(k - F) 1(k - F > 0)]
// 			    = E[(k - F) 1(F < k)]
//              = k P(F < k) - E[F 1(F < k)]
//              = k P(F < k) - f P_s(F < k)
// where dP_s/dP = exp(s X - kappa(s))
#pragma once
#include <cmath>
#include <limits>
#include <tuple>
namespace fms::option {
	// Return Not a Number instead of throwing exception.
	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();
	// Interface for option pricing models. 
	template<class F = double, class S = double>
	struct model {
		using T = std::common_type_t<F, S>;
		// Cumulative share distribution function
		// P_s(X < x) = E[1(X < x) exp(s X - kappa(s))]
		T cdf(F x, S s) const
		{
			return _cdf(x, s);
		}
		// Cumulant generating function
		// kappa(s) = log E[exp(s X)]
		S cgf(S s) const
		{
			return _cgf(s);
		}
	private:
		virtual T _cdf(F x, S s) const = 0;
		virtual S _cgf(S s) const = 0;
	};

	namespace black {
		// F < k iff X < (log(k/f) + kappa(s))/s
		template<class F = double, class S = double, class K = double>
		auto moneyness(F f, S s, K k, const model<F, S>& m)
		{
			using T = model<F, S>::T;
			if (f <= 0 or s <= 0 or k <= 0) {
				return NaN<T>;
			}
			return (std::log(k / f) + m.cgf(s)) / s;
		}
		template<class F = double, class S = double, class K = double>
		auto put(F f, S s, K k, const model<F, S>& m)
		{
			auto x = moneyness(f, s, k, m);
			return k * m.cdf(x, 0) - f * m.cdf(x, s);
		}

		// put-call parity: call = put + f - k
		// (F - k)^+ - (k - F)^+ = F - k
		template<class F = double, class S = double, class K = double>
		auto call(F f, S s, K k, const model<F, S>& m)
		{
			return put(f, s, k, m) + f - k;
		}
		// In the Black-Scholes/Merton model
		// F = s0 exp(r t) exp(sigma B_t - sigma^2 t/2)
		// In the Black model
		// F = f exp(s X - s^2/2),
		// so f = s0 exp(r t) and s = sigma * sqrt(t).
		namespace bsm {
			// Return f and s Black parameters from B-S/M parameters.
			// tuple<F, S> = {(f, s) | f in F, s in S}
			// It is a runtime version of struct { F f; S s; }
			template<class F = double, class S = double>
			inline std::tuple<F, S> bsm_to_black(double r, double s0, double sigma, double t)
			{
				return { s0 * std::exp(r * t), sigma * std::sqrt(t) };
			}
			template<class F = double, class S = double>
			inline auto moneyness(double r, double s0, double sigma, double k, double t,
				const model<F, S>& m)
			{
				auto [f, s] = bsm_to_black(s0, r, sigma, t);

				return black::moneyness(f, s, k, m);
			}
			template<class F = double, class S = double>
			inline auto put(double r, double s0, double sigma, double k, double t,
				const model<F, S>& m)
			{
				auto [f, s] = bsm_to_black<F, S>(r, s0, sigma, t);
				return black::put(f, s, k, m);
			}
			template<class F = double, class S = double>
			inline auto call(double r, double s0, double sigma, double k, double t,
				const model<F, S>& m)
			{
				auto [f, s] = bsm_to_black<F, S>(r, s0, sigma, t);
				return black::call(f, s, k, m);
			}
		}
	}
} // namespace fms