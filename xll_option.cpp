// xll_option.cpp - Generalized option model
#include "fms_option.h"
#include "xll_ml.h"
#undef CATEGORY
#define CATEGORY L"OPTION"
using namespace xll;
using namespace fms::option;
AddIn xai_option_cdf(
	Function(XLL_DOUBLE, L"xll_option_cdf", CATEGORY L".CDF")
	.Arguments({
		Arg(XLL_DOUBLE, L"x", L"is the option price."),
		Arg(XLL_DOUBLE, L"s", L"is the volatility."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return cumulative share distribution function value of an option.")
);
double WINAPI xll_option_cdf(double x, double s, HANDLEX m)
{
#pragma XLLEXPORT
	double result = NaN<double>;

	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = m_->cdf(x, s);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}
AddIn xai_option_cgf(
	Function(XLL_DOUBLE, L"xll_option_cgf", CATEGORY L".CGF")
	.Arguments({
		Arg(XLL_DOUBLE, L"s", L"is the volatility."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return cumulant generating function value of an option.")
);
double WINAPI xll_option_cgf(double s, HANDLEX m)
{
#pragma XLLEXPORT
	double result = NaN<double>;

	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = m_->cgf(s);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}
AddIn xai_black_moneyness(
	Function(XLL_DOUBLE, L"xll_black_moneyness", CATEGORY L".BLACK.MONEYNESS")
	.Arguments({
		Arg(XLL_DOUBLE, L"f", L"is the forward price."),
		Arg(XLL_DOUBLE, L"s", L"is the volatility."),
		Arg(XLL_DOUBLE, L"k", L"is the strike price."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return moneyness of an option.")
);
double WINAPI xll_black_moneyness(double f, double s, double k, HANDLEX m)
{
#pragma XLLEXPORT
	double result = NaN<double>;
	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = black::moneyness(f, s, k, *m_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}
AddIn xai_option_black_put(
	Function(XLL_DOUBLE, L"xll_option_black_put", CATEGORY L".BLACK.PUT")
	.Arguments({
		Arg(XLL_DOUBLE, L"f", L"is the forward price."),
		Arg(XLL_DOUBLE, L"s", L"is the volatility."),
		Arg(XLL_DOUBLE, L"k", L"is the strike price."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return price of a European put option under the model.")
);
double WINAPI xll_option_black_put(double f, double s, double k, HANDLEX m)
{
#pragma	XLLEXPORT
	double result = NaN<double>;
	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = black::put(f, s, k, *m_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}
AddIn xai_option_black_call(
	Function(XLL_DOUBLE, L"xll_option_black_call", CATEGORY L".BLACK.CALL")
	.Arguments({
		Arg(XLL_DOUBLE, L"f", L"is the forward price."),
		Arg(XLL_DOUBLE, L"s", L"is the volatility."),
		Arg(XLL_DOUBLE, L"k", L"is the strike price."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return price of a European call option under the model.")
);
double WINAPI xll_option_black_call(double f, double s, double k, HANDLEX m)
{
#pragma XLLEXPORT
	double result = NaN<double>;
	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = black::call(f, s, k, *m_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}
AddIn xai_option_bsm_put(
	Function(XLL_DOUBLE, L"xll_option_bsm_put", CATEGORY L".BSM.PUT")
	.Arguments({
		Arg(XLL_DOUBLE, L"r", L"is the risk-free rate."),
		Arg(XLL_DOUBLE, L"s0", L"is the spot price."),
		Arg(XLL_DOUBLE, L"sigma", L"is the volatility."),
		Arg(XLL_DOUBLE, L"k", L"is the strike price."),
		Arg(XLL_DOUBLE, L"t", L"is the time to expiration in years."),
		Arg(XLL_HANDLEX, L"m", L"is the handle to a model."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return price of a European put option under the Black-Scholes/Merton model.")
);
double WINAPI xll_option_bsm_put(double r, double s0, double sigma, double k, double t, HANDLEX m)
{
#pragma XLLEXPORT
	double result = NaN<double>;
	try {
		handle<model<>> m_(m);
		ensure(m_);
		result = black::bsm::put(r, s0, sigma, k, t, *m_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
	catch (...) {
		XLL_ERROR(__FUNCTION__ ": unknown exception");
	}
	return result;
}