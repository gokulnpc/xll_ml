// fms_curve_pwflat.h - Piecewise flat forward curve value type.
#pragma once
#include <compare>
#include <stdexcept>
#include <span>
#include <vector>
#include "fms_error.h"
#include "fms_pwflat.h"
#include "fms_curve.h"

namespace fms::curve {

	template<class T = double, class F = double>
	class pwflat : public base<T, F> {
		std::vector<T> t_;
		std::vector<F> f_;
	public:
		// constant curve
		constexpr pwflat()
		{ }
		pwflat(size_t n, const T* t, const F* f)
			: t_(t, t + n), f_(f, f + n)
		{
			ensure(fms::pwflat::monotonic(n, t));
		}
		pwflat(std::span<T> t, std::span<F> f)
			: t_(t.begin(), t.end()), f_(f.begin(), f.end())
		{
			ensure(t_.size() == f_.size() || !"pwflat: t and f must have the same size");
		}
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		pwflat(pwflat&&) = default;
		pwflat& operator=(pwflat&&) = default;
		virtual ~pwflat() = default;

		// Equal values.
		bool operator==(const pwflat& c) const
		{
			return t_ == c.t_ && f_ == c.f_;
		}

		F _forward(T u) const noexcept override
		{
			return fms::pwflat::forward(u, t_.size(), t_.data(), f_.data());
		}
		F _integral(T u) const noexcept override
		{
			return fms::pwflat::integral(u, t_.size(), t_.data(), f_.data());
		}

		bool clear() noexcept
		{
			bool empty = t_.empty() and f_.empty();

			t_.clear();
			f_.clear();

			return empty;
		}
		std::size_t size() const
		{
			return t_.size();
		}
		const T* time() const
		{
			return t_.data();
		}
		const F* rate() const
		{
			return f_.data();
		}

		pwflat& push_back(T t, F f)
		{
			ensure(size() == 0 || t >= t_.back());

			t_.push_back(t);
			f_.push_back(f);

			return *this;
		}
		pwflat& push_back(std::pair<T, F> p)
		{
			return push_back(p.first, p.second);
		}
		std::pair<T, F> back() const
		{
			return { t_.back(), f_.back() };
		}
	};

#ifdef _DEBUG
	inline int pwflat_test()
	{
		{
			// more constexpr tests.
			pwflat<> c;
			auto c2(c);
			assert(c == c2);
			c2 = c;
			assert(!(c2 != c));
		}

		return 0;
	}
#endif // _DEBUG
} // namespace fms::curve