#include "AEXMath.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_float.hpp>
namespace aex
{

	Quaternion::Quaternion()
		: x(0), y(0), z(0), w(1.0f)
	{}
	Quaternion::Quaternion(float xx, float yy, float zz, float ww)
		: x(xx), y(yy), z(zz), w(ww)
	{}
	Quaternion::Quaternion(AEVec3 vec, float ww)
		: x(vec.x), y(vec.y), z(vec.z), w(ww)
	{}
	Quaternion::Quaternion(AEVec3 vec)
		: x(vec.x), y(vec.y), z(vec.z), w(0.0f)
	{}


	// operators
	Quaternion Quaternion::operator+ (const Quaternion & rhs)const 
	{
		Quaternion res;
		for (unsigned int i = 0; i < 4; ++i)
			res.v[i] = v[i] + rhs.v[i];
		return res;
	}
	Quaternion Quaternion::operator- (const Quaternion & rhs)const
	{
		Quaternion res;
		for (unsigned int i = 0; i < 4; ++i)
			res.v[i] = v[i] - rhs.v[i];
		return res;
	}
	Quaternion& Quaternion::operator+= (const Quaternion & rhs)
	{
		for (unsigned int i = 0; i < 4; ++i)
			v[i] = v[i] + rhs.v[i];
		return *this;
	}
	Quaternion& Quaternion::operator-=(const Quaternion &rhs)
	{
		for (unsigned int i = 0; i < 4; ++i)
			v[i] = v[i] - rhs.v[i];
		return *this;
	}
	Quaternion Quaternion::operator*(float scalar)const 
	{
		Quaternion res;
		for (unsigned int i = 0; i < 4; ++i)
			res.v[i] = v[i] *scalar;
		return res;
	}
	Quaternion Quaternion::operator/(float scalar)const 
	{
		Quaternion res;
		for (unsigned int i = 0; i < 4; ++i)
			res.v[i] = v[i] / scalar;
		return res;
	}
	Quaternion& Quaternion::operator*=(float scalar)
	{
		for (unsigned int i = 0; i < 4; ++i)
			v[i] = v[i] * scalar;
		return *this;
	}
	Quaternion& Quaternion::operator/=(float scalar)
	{
		for (unsigned int i = 0; i < 4; ++i)
			v[i] = v[i] / scalar;
		return *this;
	}

	// Left to right concatenation (q = this * rhs).
	Quaternion Quaternion::operator *(const Quaternion&rhs)const
	{
		glm::quat q1,q2;// { x, y, z, w };
		q1.x = x; q1.y = y; q1.z = z; q1.w = w;
		q2.x = rhs.x; q2.y = rhs.y; q2.z = rhs.z; q2.w = rhs.w;
		glm::quat res = glm::normalize(q1 * q2);

		return Quaternion(res.x, res.y, res.z, res.w);

		//AEVec3 v1 = { x, y, z };
		//AEVec3 v2 = { rhs.x, rhs.y, rhs.z };
		//
		//float ww = w * rhs.w - v1*v2;
		//AEVec3 vec = v2*w + v1*rhs.w + v1.Cross(v2);
		//
		//return Quaternion(vec, ww).Normalize();
	}
	Quaternion& Quaternion::operator*=(const Quaternion&rhs)
	{
		return *this = *this * rhs;
	}

	// vector rotation
	AEVec3 Quaternion::operator*(const AEVec3 & rhs)const
	{
		glm::vec3 v(rhs.x, rhs.y, rhs.z);
		glm::quat q(w, x, y, z);
		glm::vec3 r = q * v;
		return AEVec3(r.x, r.y, r.z);

		// normalize the quaternion
		//Quaternion qn = Normalize();
		//float vMult = 2.0f*(x*rhs.x + y*rhs.y + z*rhs.z);
		//float crossMult = 2.0f*w;
		//float pMult = crossMult*w - 1.0f;

		//// return vector
		//return AEVec3(	pMult*rhs.x + vMult *x + crossMult*(y*rhs.z - z*rhs.y),
		//				pMult*rhs.y + vMult *y + crossMult*(z*rhs.x - x*rhs.z),
		//				pMult*rhs.z + vMult *z + crossMult*(x*rhs.y - y*rhs.x));

		 // Extract the vector part of the quaternion
		//Quaternion q = Normalize();
		//AEVec3 u(q.x, q.y, q.z);
		//AEVec3 v = rhs;

		//// Extract the scalar part of the quaternion
		//float s = q.w;

		//// Do the math
		//AEVec3 vprime = 2.0f * (u * (u * v))
		//	+ v * (s*s - (u * u))
		//	+ 2.0f * s * u.Cross(v);
		//return vprime;
	}

	// common operations
	void Quaternion::SetIdentity()
	{
		w = 1.0f;
		x = y = z = 0.0f;
	}
	float Quaternion::Length()const
	{
		float res = 0;
		for (unsigned int i = 0; i < 4; ++i)
			res += v[i] * v[i];
		return sqrtf(res);
	}
	Quaternion Quaternion::Normalize()const
	{
		return *this / Length();
	}
	void Quaternion::NormalizeThis()
	{
		*this /= Length();
	}
	float Quaternion::Dot(const Quaternion &rhs)const
	{
		float res = 0;
		for (unsigned int i = 0; i < 4; ++i)
			res += v[i] * rhs.v[i];
		return res;
	}
	Quaternion Quaternion::Inverse()const
	{
		glm::quat q;
		q.x = x; q.y = y; q.z = z; q.w = w;
		q = glm::inverse(q);
		return Quaternion(q.x, q.y, q.z, q.w);
		//return Quaternion(x, y, z, -w);
	}

	// Conversion to Matrix
	AEMtx33 Quaternion::ToMtx33()const 
	{
		AEMtx33 res;
		//float s = 2.0f / (x*x + y*y + z*z + w*w);

		//float xs = x*s; float ys = y*s; float zs = z*s;
		//float wx = w*xs; float wy = w*ys; float wz = w*zs;
		//float xx = x*xs; float xy = x*ys; float xz = x*zs;
		//float yy = y*ys; float yz = y*zs; float zz = z*zs;

		//res.v[0] = 1.0f - (yy + zz);
		//res.v[3] = xy - wz;
		//res.v[6] = xz + wy;

		//res.v[1] = xy + wz;
		//res.v[4] = 1.0f - (xx + zz);
		//res.v[7] = yz - wx;

		//res.v[2] = xz - wy;
		//res.v[5] = yz + wx;
		//res.v[8] = 1.0f - (xx + yy);

		glm::quat q;
		q.x = x; q.y = y; q.z = z; q.w = w;
		glm::mat3 mat = glm::toMat3(q);
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				res.m[i][j] = mat[j][i];

		return res;
	}
	AEMtx44 Quaternion::ToMtx44()const 
	{
		glm::quat q;
		q.x = x; q.y = y; q.z = z; q.w = w;
		glm::mat4 mat = glm::toMat4(q);
		AEMtx44 rotMat;
		for (unsigned int i = 0; i < 4; ++i)
			for (unsigned int j = 0; j < 4; ++j)
			rotMat.m[i][j] = mat[j][i];

		return rotMat;
	}
	AEVec3	Quaternion::ToEulerXYZ() const
	{
		AEVec3 euler;
		// convert the transform's quaternoin to euler angles
		AEMtx44 rot44 = this->ToMtx44();
		rot44.GetEulerAnglesXYZ(euler.x, euler.y, euler.z);

		// return
		return euler;
	}

	Quaternion& Quaternion::FromMtx33(const AEMtx33& mtx33)
	{
		return *this = MakeMatrix33(mtx33);
	}
	Quaternion& Quaternion::FromMtx44(const AEMtx44& mtx44)
	{
		return *this = MakeMatrix33(mtx44);
	}
	Quaternion& Quaternion::FromAxisAngle(const AEVec3 & axis, float angle_rad)
	{
		return *this = MakeAxisAngle(axis, angle_rad);
	}
	Quaternion& Quaternion::FromEulerXYZ(float x_rad, float y_rad, float z_rad)
	{
		return *this = MakeEulerXYZ(x_rad, y_rad, z_rad);
	}

	Quaternion Quaternion::MakeAxisAngle(const AEVec3 &axis, float angle_rad)
	{
		Quaternion res;
		res.w = Cos(angle_rad / 2.0f);
		AEVec3 a = axis*Sin(angle_rad / 2.0f);
		res.x = a.x; res.y = a.y; res.z = a.z;
		res.NormalizeThis();
		return res;
	}
	Quaternion Quaternion::MakeEulerXYZ(float x_rad, float y_rad, float z_rad)
	{
		float hx = x_rad *0.5f, hy = y_rad *0.5f, hz = z_rad *0.5f;

		Quaternion qx(Sin(hx), 0, 0, Cos(hx));
		Quaternion qy(0, Sin(hy), 0, Cos(hy));
		Quaternion qz(0, 0, Sin(hz), Cos(hz));

		return (qz*qy*qx).Normalize();
		//return qx*qy*qz;
	}
	Quaternion Quaternion::MakeMatrix33(const AEMtx33 & rot33)
	{
		// get the trace of the matrix
		float trace = rot33.m[0][0] + rot33.m[1][1] + rot33.m[2][2];

		if (trace > 0.0f) // positive trace
		{
			// trace of rotation matrix is equal to 2cos(angle) + 1
			float qw = trace + 1.0f;
			// axis is given by: 
			AEVec3 axis = { rot33.m[2][1] - rot33.m[1][2], rot33.m[0][2] - rot33.m[2][0], rot33.m[1][0] - rot33.m[0][1] };

			//normalize to get pure rotation quaternion.
			return Quaternion(axis, qw).Normalize();
		}
		else // negative trace
		{
			float qx, qy, qz, qw;
			// find the largest diagonal element
			if (rot33.m11 > rot33.m22 && rot33.m11 > rot33.m33) // largest element is top-left. 
			{
				qx = rot33.m11 - rot33.m22 - rot33.m33 + 1;
				qy = rot33.m12 + rot33.m21;
				qz = rot33.m13 + rot33.m31;
				qw = rot33.m32 - rot33.m23;
			}
			else if (rot33.m22 > rot33.m11 && rot33.m22 > rot33.m33) // largest element is middle.
			{
				qy = rot33.m22 - rot33.m11 - rot33.m33 + 1;
				qx = rot33.m12 + rot33.m21;
				qz = rot33.m23 + rot33.m32;
				qw = rot33.m13 - rot33.m31;
			}
			else // largest diagonal element is bottom-right. 
			{
				qz = rot33.m33 - rot33.m11 - rot33.m22 + 1;
				qx = rot33.m13 + rot33.m31;
				qy = rot33.m32 + rot33.m23;
				qw = rot33.m21 - rot33.m12;
			}

			// normalize and return
			return Quaternion(qx, qy, qz, qw).Normalize();
		}
	}
	Quaternion Quaternion::MakeLookAt(const AEVec3& forward, const AEVec3& up)
	{
		glm::vec3 fwd = { forward.x, forward.y, forward.z };
		glm::vec3 u = { up.x, up.y, up.z };
		glm::quat q = glm::quatLookAt(fwd, u);

		return Quaternion(q.x, q.y, q.z, q.w);
	}
} // namespace aex