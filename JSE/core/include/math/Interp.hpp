#ifndef JSE_INTERP_H
#define JSE_INTERP_H

#include <cmath>
#include "system/SystemTypes.hpp"

namespace jse {

	template <class T, class Q>
	inline T LinearInterp(const T& p1, const T& p2, const Q f)
	{
		return (static_cast<Q>(1) - f) * p1 + f * p2;
	}

	template<class T, class Q>
	inline T CosineInterp(const T& p1, const T& p2, const Q f)
	{
		const Q f2 = (static_cast<Q>(1) - cosf(f * M_PI)) / static_cast<Q>(2);
		return LinearInterp(p1, p2, f2);
	}

	/*
   Tension: 1 is high, 0 normal, -1 is low
   Bias: 0 is even,
		 positive is towards first segment,
		 negative towards the other
*/
	template<class T, class Q>
	inline T HermiteInterp(
		const T& y0, const T& y1,
		const T& y2, const T& y3,
		const Q mu,
		const Q tension,
		const Q bias)
	{
		Q mu2, mu3, a0, a1, a2, a3;
		T m1, m0;

		const Q _1 = static_cast<Q>(1);
		const Q _2 = static_cast<Q>(2);
		const Q _3 = static_cast<Q>(3);

		mu2 = mu * mu;
		mu3 = mu2 * mu;
		m0 = (y1 - y0) * (_1 + bias) * (_1 - tension) / _2;
		m0 += (y2 - y1) * (_1 - bias) * (_1 - tension) / _2;
		m1 = (y2 - y1) * (_1 + bias) * (_1 - tension) / _2;
		m1 += (y3 - y2) * (_1 - bias) * (_1 - tension) / _2;
		a0 = _2 * mu3 - _3 * mu2 + _1;
		a1 = mu3 - _2 * mu2 + mu;
		a2 = mu3 - mu2;
		a3 = -_2 * mu3 + _3 * mu2;

		return(a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
	}


	template<class T, class Q>
	inline T CubicInterp(
		const T& y0, const T& y1,
		const T& y2, const T& y3,
		const Q mu)
	{
		const Q mu2 = mu * mu;
		const T a0 = y3 - y2 - y0 + y1;
		const T a1 = y0 - y1 - a0;
		const T a2 = y2 - y0;
		//a3 = y1;

		return(a0 * mu * mu2 + a1 * mu2 + a2 * mu + y1 /*a3*/);
	}

	template<class T, class Q>
	inline T CatmullRomInterp(
		const T& y0, const T& y1,
		const T& y2, const T& y3,
		const Q mu)
	{
		const Q _0_5 = static_cast<Q>(0.5);
		const Q _1_5 = static_cast<Q>(1.5);
		const Q _2_5 = static_cast<Q>(2.5);
		const Q _2 = static_cast<Q>(2);

		const Q mu2 = mu * mu;
		const T a0 = -_0_5 * y0 + _1_5 * y1 - _1_5 * y2 + _0_5 * y3;
		const T a1 = y0 - _2_5 * y1 + _2 * y2 - _0_5 * y3;
		const T a2 = -_0_5 * y0 + _0_5 * y2;		
		//a3 = y1;

		return(a0 * mu * mu2 + a1 * mu2 + a2 * mu + y1 /*a3*/);
	}

	Quat QuatNormalize(const Quat& a0)
	{
		const float length = 1.0f / sqrtf(a0.x * a0.x + a0.y * a0.y + a0.z * a0.z + a0.w * a0.w);

		if (length <= 0.0f) return Quat(1.0f, 0.0f, 0.0f, 0.0f);

		return a0 * length;
	}
}

#endif