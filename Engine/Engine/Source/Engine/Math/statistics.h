#pragma once

#include <cmath>
#include <concepts>
#include <stdexcept>

namespace ph::math
{

/*! @brief Computes regularized lower incomplete gamma function.
"A Set of Algorithms for the Incomplete Gamma Functions" by N. M. Temme is also a good read on
this topic.
*/
template<std::floating_point T>
inline T regularized_lower_incomplete_gamma(const T a, const T x)
{
	// The following implementation is from pbrt-v4: https://github.com/mmp/pbrt-v4/blob/f140d7cba5dc7b941f9346d6b7d1476a05c28c37/src/pbrt/bsdfs_test.cpp#L56
	// Some changes are made to fit our style. it still computes in `double` no matter what `T` is.

	constexpr double epsilon = 0.000000000000001;
	constexpr double big     = 4503599627370496.0;
	constexpr double bigRcp  = 2.22044604925031308085e-16;

	if(a < 0 || x < 0)
	{
		throw std::runtime_error("`a` and `x` cannot be negative");
	}

	if(x == 0)
	{
		return 0;
	}

	double ax = (a * std::log(x)) - x - std::lgamma(a);
	if(ax < -709.78271289338399)
	{
		return a < x ? 1.0 : 0.0;
	}

	if(x <= 1 || x <= a)
	{
		double r2   = a;
		double c2   = 1;
		double ans2 = 1;

		do
		{
			r2 = r2 + 1;
			c2 = c2 * x / r2;
			ans2 += c2;
		}
		while((c2 / ans2) > epsilon);

		return std::exp(ax) * ans2 / a;
	}

	int    c   = 0;
	double y   = 1 - a;
	double z   = x + y + 1;
	double p3  = 1;
	double q3  = x;
	double p2  = x + 1;
	double q2  = z * x;
	double ans = p2 / q2;
	double error;
	do
	{
		++c;
		y += 1;
		z += 2;
		double yc = y * c;
		double p  = (p2 * z) - (p3 * yc);
		double q  = (q2 * z) - (q3 * yc);

		if(q != 0)
		{
			double nextAns = p / q;
			error = std::abs((ans - nextAns) / nextAns);
			ans = nextAns;
		}
		else
		{
			// Division by zero, skip
			error = 1;
		}

		// Shift
		p3 = p2;
		p2 = p;
		q3 = q2;
		q2 = q;

		// Normalize fraction when the numerator becomes large
		if(std::abs(p) > big)
		{
			p3 *= bigRcp;
			p2 *= bigRcp;
			q3 *= bigRcp;
			q2 *= bigRcp;
		}
	}
	while(error > epsilon);

	return static_cast<T>(1 - (std::exp(ax) * ans));
}

/*!
@param x Chi^2 value.
@param dof Degrees of freedom.
*/
template<std::floating_point T, std::integral DofType>
inline T chi2_CDF(T x, DofType dof)
{
	if(dof < 1 || x < 0)
	{
		return 0;
	}
	else if(dof == 2)
	{
		return static_cast<T>(1 - std::exp(-0.5 * x));
	}
	else
	{
		return static_cast<T>(regularized_lower_incomplete_gamma(0.5 * dof, 0.5 * x));
	}
}

}// end namespace ph::math
