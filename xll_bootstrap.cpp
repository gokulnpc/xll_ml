// xll_bootstrap.cpp - bootstrap functions
#include "xll_fi.h"
#include "fms_bootstrap.h"
using namespace xll;
using namespace fms;

static AddIn xai_fi_curve_bootstrap(
	Function(XLL_HANDLEX, L"xll_fi_curve_bootstrap", L"\\" CATEGORY L".CURVE.PWFLAT.BOOTSTRAP")
	.Arguments({
		Arg(XLL_FP, L"instruments", L"is an array of handles to instruments."),
		Arg(XLL_FP, L"prices", L"is an array of instrument prices."),
		})
		.Uncalced()
	.Category(CATEGORY)
	.FunctionHelp(L"Return a handle to a bootstrapped piecewise flat forward curve.")
);
HANDLEX WINAPI xll_fi_curve_bootstrap(_FP12* pi, _FP12* pp)
{
#pragma XLLEXPORT
	HANDLEX h = INVALID_HANDLEX;
	try {
		ensure(size(*pi) == size(*pp));
		int n = size(*pi);
		std::vector<instrument::base<>*> is(n);
		for (int i = 0; i < n; ++i) {
			handle<instrument::base<>> h_(static_cast<HANDLEX>(pi->array[i]));
			ensure(h_);
			is[i] = h_.ptr();
		}
		std::span<double> ps(pp->array, n);
		curve::pwflat<> f;
		double _t = 0, _f = 0.03;
		for (int i = 0; i < n; ++i) {
			std::tie(_t, _f) = curve::bootstrap0(*is[i], f, _t, _f, ps[i]);
			f.push_back(_t, _f);
		}
		handle<curve::base<>> h_(new curve::pwflat<>(f));
		ensure(h_);
		h = h_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return h;
}