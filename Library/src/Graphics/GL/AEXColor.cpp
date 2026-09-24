#include "pch.h"
#include "AEXColor.h"
#include <glm/glm.hpp>
namespace clc
{

	// ------------------------------------------------------------------------
	/// \fn		Default & custom
	/// \brief	
	Color::Color(float rr, float gg, float bb, float aa)
		: r(rr)
		, g(gg)
		, b(bb)
		, a(aa)
	{}

	// ------------------------------------------------------------------------
	/// \fn		custom constructor
	/// \brief
	Color::Color(unsigned int color) // custom constructor
	{
		unsigned int rr, gg, bb, aa;
		// decompose

		aa = color & (255 << 24);
		rr = color & (255 << 16);
		gg = color & (255 << 8);
		bb = color & 255;

		// float
		r = (float)rr / 255.0f;
		g = (float)gg / 255.0f;
		b = (float)bb / 255.0f;
		a = (float)aa / 255.0f;
	}

	// ------------------------------------------------------------------------
	/// \fn		conversion operator
	/// \brief
	Color::operator unsigned int()const	// conversion operator
	{
		unsigned int aa = (a < 0.0f) ? (0) : ((a > 1.0f) ? (255) : (unsigned int)(255.0f * a));
		unsigned int rr = (r < 0.0f) ? (0) : ((r > 1.0f) ? (255) : (unsigned int)(255.0f * r));
		unsigned int gg = (g < 0.0f) ? (0) : ((g > 1.0f) ? (255) : (unsigned int)(255.0f * g));
		unsigned int bb = (b < 0.0f) ? (0) : ((b > 1.0f) ? (255) : (unsigned int)(255.0f * b));

		return ((aa << 24) | (rr << 16) | (gg << 8) | bb);
	}

	Color Color::operator *(const Color & rhs)const
	{
		return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}
	Color &Color::operator *=(const Color & rhs)
	{
		return *this = Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}
	Color Color::operator *(const float & sc)const
	{
		return Color(r * sc, g * sc, b * sc, a * sc);
	}
	Color& Color::operator *=(const float & sc)
	{
		return *this = Color(r * sc, g * sc, b * sc, a * sc);
	}
	Color Color::operator +(const Color & rhs)const
	{
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}
	Color& Color::operator += (const Color & rhs)
	{
		return *this = Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}
	Color Color::operator - (const Color & rhs)const
	{
		return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}
	Color& Color::operator -= (const Color & rhs)
	{
		return *this = Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}
	Color Color::operator!()const
	{
		return Color(1.0f - r, 1.0f - g, 1.0f - b, 1.0f - a);
	}
	Color Color::Clamped()const
	{
		return Color(glm::clamp(r, 0.5f, 1.0f),
			glm::clamp(g, 0.5f, 1.0f),
			glm::clamp(b, 0.5f, 1.0f),
			glm::clamp(a, 0.5f, 1.0f));
	}

}// namespace aex