// fms_instrument.h - header file for the FMS Instrument class
#pragma once
#include <algorithm>
#include <span>
#include <vector>
#include "fms_error.h"
namespace fms::instrument {
	template<class U = double, class C = double>
	class base {
	public:
		base() = default;
		virtual ~base() = default;
		base(const base&) = default;
		base& operator=(const base&) = default;
		base(base&&) = default;
		base& operator=(base&&) = default;

		constexpr std::size_t size() const noexcept { return _size(); }
		constexpr const U* time() const noexcept { return _time(); }
		constexpr std::span<const C> times() const noexcept { return { _time(), _size() }; }
		constexpr const C* cash() const noexcept { return _cash(); }
		constexpr std::span<const C> cashes() const noexcept { return { _cash(), _size() }; }
		std::pair<U, C> first() const noexcept { return { _time()[0], _cash()[0] }; }
		std::pair<U, C> last() const noexcept { return { _time()[_size() - 1], _cash()[_size() - 1] }; }
	private:
		constexpr virtual std::size_t _size() const noexcept = 0;
		constexpr virtual const U* _time() const noexcept = 0;
		constexpr virtual const C* _cash() const noexcept = 0;
	};

	template<class U = double, class C = double>
	class instrument : public base<U, C>
	{
		std::vector<U> u;
		std::vector<C> c;
	public:
		constexpr instrument(const std::vector<U>& u, const std::vector<C>& c)
			: u(u), c(c)
		{
			ensure(u.size() == c.size());
			ensure(std::is_sorted(u.begin(), u.end()));
		}
		constexpr instrument(std::span<U> u, std::span<C> c)
			: u(u.begin(), u.end()), c(c.begin(), c.end())
		{
			ensure(u.size() == c.size());
			ensure(std::is_sorted(u.begin(), u.end()));
		}
		constexpr instrument(const instrument& z) = default;
		constexpr instrument& operator=(const instrument& z) = default;
		virtual ~instrument() = default;
		constexpr std::size_t _size() const noexcept override { return u.size(); }
		constexpr const U* _time() const noexcept override { return u.data(); }
		constexpr const C* _cash() const noexcept override { return c.data(); }
	};

	template<class U = double, class C = double>
	class zero_coupon_bond : public instrument<U, C>
	{
	public:
		zero_coupon_bond(U u, C c = C(1))
			: instrument<U, C>(std::span(&u, 1), std::span(&c, 1))
		{
		}
		zero_coupon_bond(const zero_coupon_bond& z) = default;
		zero_coupon_bond& operator=(const zero_coupon_bond& z) = default;
		virtual ~zero_coupon_bond() = default;
	};

	enum class frequency {
		annual = 1,
		semiannual = 2,
		quarterly = 4,
		monthly = 12
	};

	template<class U = double>
	inline std::vector<U> periods(U u, frequency f)
	{
		std::vector<U> t{ u };
		while ((u -= U(1) / U(f)) > 0) {
			t.insert(t.begin(), u);
		}
		return t;
	}

	template<class U = double, class C = double>
	inline std::vector<C> payments(U u, C c, frequency f = frequency::semiannual)
	{
		std::size_t n = static_cast<std::size_t>(u * U(f));
		std::vector<C> p(n, c / U(f));
		p.back() += C(1);
		return p;
	}

	template<class U = double, class C = double>
	class bond : public instrument<U, C>
	{
		U u;
		C c;
		frequency f;
	public:
		bond(U u, C c, frequency f = frequency::semiannual)
			: instrument<U, C>(periods(u, f), payments(u, c, f)), u(u), c(c), f(f)
		{
		}
		bond(const bond& b) = default;
		bond& operator=(const bond& b) = default;
		virtual ~bond() = default;
	};
} // namespace fms