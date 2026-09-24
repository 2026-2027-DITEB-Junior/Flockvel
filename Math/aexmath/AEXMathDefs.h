#ifndef AEX_MATH_DEFS_H_
#define AEX_MATH_DEFS_H_

#ifndef EPSILON2
#define	EPSILON2	0.00001f
#endif

#ifndef FLOAT_ZERO
#define FLOAT_ZERO(x) (x >-EPSILON2 && x < EPSILON2)
#endif

#ifndef FLOAT_EQUAL
#define FLOAT_EQUAL(x,y) FLOAT_ZERO((x)-(y))
#endif

#ifndef DOUBLE_EPSILON
#define	DOUBLE_EPSILON	0.00001
#endif

#ifndef DOUBLE_ZERO
#define DOUBLE_ZERO(x) (x >-DOUBLE_EPSILON && x < DOUBLE_EPSILON)
#endif

#ifndef PI
#define	PI		3.1415926535897932384626433832795f
#endif

#define	HALF_PI	(PI * 0.5f)
#define	TWO_PI	(PI * 2.0f)

#endif