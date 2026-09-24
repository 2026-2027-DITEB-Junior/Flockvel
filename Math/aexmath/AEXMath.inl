#ifndef AEX_MATH_INL
#define AEX_MATH_INL

namespace aex
{
	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float DegToRad(float x)
	{
		return x * (PI / 180.0f);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float RadToDeg(float x)
	{
		return x * (180.0f / PI);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Sin(float x)
	{
		return sinf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Cos(float x)
	{
		return cosf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Tan(float x)
	{
		return tanf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ASin(float x)
	{
		return asinf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ACos(float x)
	{
		return acosf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ATan(float x)
	{
		return atanf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float SinDeg(float x)
	{
		return Sin(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float CosDeg(float x)
	{
		return Cos(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float TanDeg(float x)
	{
		return Tan(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ASinDeg(float x)
	{
		return ASin(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ACosDeg(float x)
	{
		return ACos(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float ATanDeg(float x)
	{
		return ATan(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API unsigned int IsPowOf2(unsigned int x)
	{
		return ((x) && (((x)& ((x)-1)) == 0));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API unsigned int NextPowOf2(unsigned int x)
	{
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		return x + 1;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API unsigned int LogBase2(unsigned int x)
	{
		unsigned int i;

		for (x = x >> 1, i = 0; x >> i; i++)
			;

		return i;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Wrap(float x, float x0, float x1)
	{
		if (x < x0)
			return x + (x1 - x0);
		if (x > x1)
			return x - (x1 - x0);
		return x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Min(float x, float y)
	{
		return (x < y) ? x : y;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Max(float x, float y)
	{
		return (x < y) ? y : x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API int Min(int x, int y)
	{
		return (x < y) ? x : y;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API int Max(int x, int y)
	{
		return (x < y) ? y : x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API float Clamp(float x, float x0, float x1)
	{
		return Max(x0, Min(x, x1));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API int Clamp(int x, int x0, int x1)
	{
		return Max(x0, Min(x, x1));
	}

	// ---------------------------------------------------------------------------

	inline bool AEX_MATH_API InRange(float x, float x0, float x1)
	{
		if ((x0 <= x) && (x <= x1))
			return true;

		return false;
	}

	// ---------------------------------------------------------------------------
	
	inline float AEX_MATH_API RandFloat(float min, float max)
	{
		int r = rand();
		return min + ((float)r/(float)RAND_MAX) * (max - min);
	}

	inline float AEX_MATH_API RandFloat01()
	{
		int r = rand();
		return ((float)r / (float)RAND_MAX);
	}

	template <typename T>
	T  Lerp(T start, T end, float tn)
	{
		return start + (end - start)*tn;
	}
}

// ---------------------------------------------------------------------------
#endif