// naive impl of 3d math functions
// can to be used as SWIG input
//
// matrix is row major
//
// author:   If
// birthday: 12th/Jul/2014
// license:  MIT
#pragma once

#include <cmath>
#include <limits>
#include <algorithm>
#include <stdexcept>
#ifdef _MSC_VER
#include <float.h>
#endif

#ifndef real_t
typedef float real_t;
#endif

const real_t _EPSILON = real_t(1.0e-10);
const real_t _AVOID_DIV_BY_0 = (std::numeric_limits<real_t>::min)();

inline bool almostEqual(real_t x, real_t y, real_t eps) { return std::fabs(x - y)<eps; }
inline bool equal(real_t x, real_t y) { return std::fabs(x - y)<_EPSILON; }

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct plane;
struct mat3;
struct mat4;

// vec {{{

struct vec2 {
	real_t x, y;

	vec2() :x(0.f), y(0.f) {}
	explicit vec2(real_t s) :x(s), y(s) {}
	vec2(real_t x, real_t y) :x(x), y(y) {}
	vec2(vec2 const& v) :x(v.x), y(v.y) {}

	real_t lengthSquared() const { return x*x + y*y; }
	real_t length() const { return std::sqrt(lengthSquared()); }
	real_t dot(vec2 const& v) const { return x*v.x + y*v.y; }
	vec2  normalized() const { return *this / (this->length() + _AVOID_DIV_BY_0); }
	void  normalize() { *this /= (this->length() + _AVOID_DIV_BY_0); }
	void  set(real_t x, real_t y) { this->x = x; this->y = y; }

	vec2 operator- () const { return vec2(-x, -y); }
	vec2 operator+ (vec2 const& v) const { return vec2(x + v.x, y + v.y); }
	vec2 operator- (vec2 const& v) const { return vec2(x - v.x, y - v.y); }
	void operator+=(vec2 const& v) { x += v.x; y += v.y; }
	void operator-=(vec2 const& v) { x -= v.x; y -= v.y; }
	vec2 operator* (real_t f)      const { return vec2(x*f, y*f); }
	void operator*=(real_t f) { x *= f; y *= f; }
	vec2 operator* (vec2 const& v) const { return vec2(x*v.x, y*v.y); }
	void operator*=(vec2 const& v) { x *= v.x; y *= v.y; }
	vec2 operator/ (real_t f)      const { return vec2(x / f, y / f); }
	void operator/=(real_t f) { x /= f; y /= f; }
	bool operator==(vec2 const& v) const { return equal(x, v.x) && equal(y, v.y); }
	bool operator!=(vec2 const& v) const { return !this->operator==(v); }
};

// ---------------------------------------------------------------------------

struct vec3 {
	real_t x, y, z;

	vec3() :x(0.f), y(0.f), z(0.f) {}
	explicit vec3(real_t s) :x(s), y(s), z(s) {}
	vec3(real_t x, real_t y, real_t z) :x(x), y(y), z(z) {}
	vec3(vec3 const& v) :x(v.x), y(v.y), z(v.z) {}
	vec3(vec2 const& v, real_t z) :x(v.x), y(v.y), z(z) {}

	real_t lengthSquared() const { return x*x + y*y + z*z; }
	real_t length() const { return std::sqrt(lengthSquared()); }
	real_t dot(vec3 const& v) const { return x*v.x + y*v.y + z*v.z; }
	vec3  normalized() const { return *this / (this->length() + _AVOID_DIV_BY_0); }
	void  normalize() { *this /= (this->length() + _AVOID_DIV_BY_0); }
	void  set(real_t x, real_t y, real_t z) { this->x = x; this->y = y; this->z = z; }
	void  set(vec2 const& v, real_t z) { this->x = v.x; this->y = v.y; this->z = z; }

	vec3 operator- () const { return vec3(-x, -y, -z); }
	vec3 operator+ (vec3 const& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
	vec3 operator- (vec3 const& v) const { return vec3(x - v.x, y - v.y, z - v.z); }
	void operator+=(vec3 const& v) { x += v.x; y += v.y; z += v.z; }
	void operator-=(vec3 const& v) { x -= v.x; y -= v.y; z -= v.z; }
	vec3 operator* (real_t f)      const { return vec3(x*f, y*f, z*f); }
	void operator*=(real_t f) { x *= f; y *= f; z *= f; }
	vec3 operator* (vec3 const& v) const { return vec3(x*v.x, y*v.y, z*v.z); }
	void operator*=(vec3 const& v) { x *= v.x; y *= v.y; z *= v.z; }
	vec3 operator/ (real_t f)      const { return vec3(x / f, y / f, z / f); }
	void operator/=(real_t f) { x /= f; y /= f; z /= f; }
	bool operator==(vec3 const& v) const { return equal(x, v.x) && equal(y, v.y) && equal(z, v.z); }
	bool operator!=(vec3 const& v) const { return !this->operator==(v); }

	inline vec3 operator* (mat3 const& m) const;
	inline void operator*=(mat3 const& m) { *this = this->operator*(m); }
};

// ---------------------------------------------------------------------------

struct vec4 {
	real_t x, y, z, w;

	vec4() :x(0.f), y(0.f), z(0.f), w(0.f) {}
	explicit vec4(real_t s) :x(s), y(s), z(s), w(s) {}
	vec4(vec4 const& v) :x(v.x), y(v.y), z(v.z), w(v.w) {}
	vec4(vec3 const& v, real_t w) :x(v.x), y(v.y), z(v.z), w(w) {}
	vec4(vec2 const& v, real_t z, real_t w) :x(v.x), y(v.y), z(z), w(w) {}
	vec4(real_t x, real_t y, real_t z, real_t w) :x(x), y(y), z(z), w(w) {}

	real_t lengthSquared() const { return x*x + y*y + z*z + w*w; }
	real_t length() const { return std::sqrt(lengthSquared()); }
	real_t dot(vec4 const& v) const { return x*v.x + y*v.y + z*v.z + w*v.w; }
	vec4  normalized() const { return *this / (this->length() + _AVOID_DIV_BY_0); }
	void  normalize() { *this /= (this->length() + _AVOID_DIV_BY_0); }
	void  set(real_t x, real_t y, real_t z, real_t w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	void  set(vec3 const& v, real_t w) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = w; }

	vec4 operator- () const { return vec4(-x, -y, -z, -w); }
	vec4 operator+ (vec4 const& v) const { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	vec4 operator- (vec4 const& v) const { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	void operator+=(vec4 const& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
	void operator-=(vec4 const& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	vec4 operator* (real_t f)      const { return vec4(x*f, y*f, z*f, w*f); }
	void operator*=(real_t f) { x *= f; y *= f; z *= f; w *= f; }
	vec4 operator* (vec4 const& v) const { return vec4(x*v.x, y*v.y, z*v.z, w*v.w); }
	void operator*=(vec4 const& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
	vec4 operator/ (real_t f)      const { return vec4(x / f, y / f, z / f, w / f); }
	void operator/=(real_t f) { x /= f; y /= f; z /= f; w /= f; }
	bool operator==(vec4 const& v) const { return equal(x, v.x) && equal(y, v.y) && equal(z, v.z) && equal(w, v.w); }
	bool operator!=(vec4 const& v) const { return !this->operator==(v); }
	vec3 xyz() const { return vec3(x, y, z); }
	vec2 xy()  const { return vec2(x, y); }
	vec2 zw()  const { return vec2(z, w); }

	inline vec4 operator* (mat4 const& m) const;
	inline void operator*=(mat4 const& m) { *this = this->operator*(m); }
};

// ---------------------------------------------------------------------------

inline real_t dot(vec2 const& v1, vec2 const& v2) {
	return v1.dot(v2);
}

inline real_t dot(vec3 const& v1, vec3 const& v2) {
	return v1.dot(v2);
}

inline real_t dot(vec4 const& v1, vec4 const& v2) {
	return v1.dot(v2);
}

inline vec3 cross(vec3 const& v1, vec3 const& v2) {
	return vec3(v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x);
}

inline real_t lerp(real_t      v1, real_t      v2, real_t t) { return v1*(1.0f - t) + v2*t; }
inline vec2   lerp(vec2 const& v1, vec2 const& v2, real_t t) { return v1*(1.0f - t) + v2*t; }
inline vec3   lerp(vec3 const& v1, vec3 const& v2, real_t t) { return v1*(1.0f - t) + v2*t; }
inline vec4   lerp(vec4 const& v1, vec4 const& v2, real_t t) { return v1*(1.0f - t) + v2*t; }

inline vec2   normalize(vec2 const& v) { return v.normalized(); }
inline vec3   normalize(vec3 const& v) { return v.normalized(); }
inline vec4   normalize(vec4 const& v) { return v.normalized(); }

inline bool   equal(vec2 const& v1, vec2 const& v2) { return v1 == v2; }
inline bool   equal(vec3 const& v1, vec3 const& v2) { return v1 == v2; }
inline bool   equal(vec4 const& v1, vec4 const& v2) { return v1 == v2; }
// }}}

// ---------------------------------------------------------------------------

// quat {{{

struct quat {
	real_t w, x, y, z;

	quat() :w(1.f), x(0.f), y(0.f), z(0.f) {}
	quat(quat const& q) :w(q.w), x(q.x), y(q.y), z(q.z) {}
	quat(real_t w, real_t x, real_t y, real_t z) :w(w), x(x), y(y), z(z) {}
	quat(vec4 const& v) :w(v.w), x(v.x), y(v.y), z(v.z) {}

	static inline quat identity() { return quat(1.f, 0.f, 0.f, 0.f); }
	static inline quat fromAxisAngle(vec3 const& axis, real_t radian);
	static inline quat fromMat(mat3 const& m);
	static inline quat fromMat(mat4 const& m);
	inline void        setAxisAngle(vec3 const& axis, real_t radian);
	inline real_t      angle() const { return std::acos(w)*2.f; }
	inline vec3        axis() const;

	inline real_t      lengthSquared() const { return w*w + x*x + y*y + z*z; }
	inline real_t      length() const { return std::sqrt(lengthSquared()); }
	inline quat        normalized() const { return (*this) / (this->length() + _AVOID_DIV_BY_0); }
	inline void        normalize() { *this = normalized(); }
	inline quat        exp() const;
	inline quat        log() const;
	inline quat        operator-() const { return quat(w, -x, -y, -z); }
	inline quat        operator*(quat const& q) const;
	inline void        operator*=(quat const& q) { *this = (*this)*q; }
	inline quat        operator* (real_t s) const { return quat(w*s, x*s, y*s, z*s); }
	inline void        operator*=(real_t s) { *this = (*this)*s; }
	inline quat        operator/ (real_t s) const { return quat(w / s, x / s, y / s, z / s); }
	inline void        operator/=(real_t s) { *this = (*this) / s; }
	inline quat        operator+ (quat const& q) const { return quat(w + q.w, x + q.x, y + q.y, z + q.z); }
	inline void        operator+=(quat const& q) { *this = (*this) + q; }
	inline quat        operator- (quat const& q) const { return quat(w - q.w, x - q.x, y - q.y, z - q.z); }
	inline void        operator-=(quat const& q) { *this = (*this) - q; }
};

// impl {{{
inline quat quat::fromAxisAngle(vec3 const& axis, real_t radian) {
	quat q;
	q.setAxisAngle(axis, radian);
	return q;
}

inline void quat::setAxisAngle(vec3 const& axis, real_t radian) {
	real_t l = axis.lengthSquared();
	if (l <= _EPSILON) {
		return;
	}
	real_t s = std::sin(radian / 2.f);
	real_t c = std::cos(radian / 2.f);
	vec3  a = axis / std::sqrt(l);
	*this = quat(c, s*a.x, s*a.y, s*a.z);
}

inline vec3 quat::axis() const {
	real_t const s = std::sqrt((std::max)(real_t(1.f - w*w), real_t(0.f)));
	if (s >= _EPSILON) {
		return vec3(x / s, y / s, z / s);
	}
	else {
		return vec3(1.f, 0.f, 0.f);
	}
}

inline quat quat::operator*(quat const& q) const {
	quat ans;
	ans.w = w*q.w - x*q.x - y*q.y - z*q.z;
	ans.x = w*q.x + x*q.w + y*q.z - z*q.y;
	ans.y = w*q.y - x*q.z + y*q.w + z*q.x;
	ans.z = w*q.z + x*q.y - z*q.x + z*q.w;
	return ans;
}

inline quat slerp(quat const& q1, quat const& q2, real_t t)
{
	real_t const rc = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;
	real_t const c = std::fabs(rc);
	real_t scale, invscale;
	if (c < 1.f - 0.0001f) {
		real_t const theta = std::acos(c);
		real_t const invsin = real_t(1.0) / std::sin(theta);
		scale = std::sin(real_t((1.0 - t)*theta)) * invsin;
		invscale = std::sin(t*theta) * invsin;
	}
	else {
		scale = real_t(1.0 - t);
		invscale = t;
	}
	if (rc<0)
		invscale = -invscale;

	return q1 * scale + q2 * invscale;
}

// ref http://en.wikipedia.org/wiki/Quaternion#Exponential.2C_logarithm.2C_and_power
inline quat quat::exp() const
{
	real_t const a = std::sqrt(x*x + y*y + z*z);
	real_t const c = sin(a) / a;
	return quat(std::cos(a), x*c, y*c, z*c) * std::exp(w);
}

inline quat quat::log() const
{
	real_t const a = std::sqrt(x*x + y*y + z*z + w*w);
	real_t const c = std::acos(w / a) / std::sqrt(x*x + y*y + z*z);
	return quat(std::log(a), x*c, y*c, z*c);
}
// }}}
// }}}

// ---------------------------------------------------------------------------

// mat {{{

// mat3 {{{
struct mat3 {
	real_t m[3][3];

	mat3() { *this = identity(); }
	mat3(real_t d[9]) { memcpy(m, d, 9 * sizeof(real_t)); }
	mat3(vec3 const& row0, vec3 const& row1, vec3 const& row2) {
		row(0) = row0;
		row(1) = row1;
		row(2) = row2;
	}
	mat3(mat3 const& m) { memcpy(this->m, m.m, 9 * sizeof(real_t)); }
	mat3(real_t _00, real_t _01, real_t _02,
		real_t _10, real_t _11, real_t _12,
		real_t _20, real_t _21, real_t _22) {
		m[0][0] = _00; m[0][1] = _01; m[0][2] = _02;
		m[1][0] = _10; m[1][1] = _11; m[1][2] = _12;
		m[2][0] = _20; m[2][1] = _21; m[2][2] = _22;
	}
	static mat3 identity() {
		return mat3(1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f);
	}
	static mat3 eye(real_t v) {
		return mat3(v, 0.f, 0.f,
			0.f, v, 0.f,
			0.f, 0.f, v);
	}
	static mat3 zero() {
		return mat3(0.f, 0.f, 0.f,
			0.f, 0.f, 0.f,
			0.f, 0.f, 0.f);
	}
	static inline mat3 fromQuat(quat const& q);
	static inline mat3 fromAxisAngle(vec3 axis, real_t radian);

	inline real_t      operator()(int r, int c) const;
	inline void        set(int r, int c, real_t v);
	inline vec3&       row(int r);
	inline vec3 const& row(int r) const;
	inline void        setRow(int r, vec3 const& v) { row(r) = v; }
	inline void        setColumn(int c, vec3 const& v);
	inline mat3        transposed() const;
	inline void        transpose() { *this = transposed(); }
	inline real_t      trace() const { return m[0][0] + m[1][1] + m[2][2]; }

	inline real_t      determinant() const;
	inline mat3        inversed() const;
	inline void        inverse() { *this = inversed(); }
	inline mat3        operator*(mat3 const& m) const;
	inline vec3        operator*(vec3 const& v) const;
	inline void        operator*=(mat3 const& m) { *this = this->operator*(m); }
	inline bool        operator==(mat3 const& m) const;
	inline bool        operator!=(mat3 const& m) const { return !this->operator==(m); }
	inline void        setQuat(quat const& q);

	inline quat        toQuat() const;
};

// impl {{{
inline mat3 mat3::fromQuat(quat const& q) {
	mat3 m;
	m.setQuat(q);
	return m;
}

inline mat3 mat3::fromAxisAngle(vec3 axis, real_t radian) {
	axis.normalize();
	real_t const x = axis.x, y = axis.y, z = axis.z;
	real_t const c = std::cos(radian), s = std::sin(radian);
	real_t const C = 1 - c;
	return mat3(
		x*x*C + c, x*y*C - z*s, x*z*C + y*s,
		y*x*C + z*s, y*y*C + c, y*z*C - x*s,
		z*x*C - y*s, z*y*C + x*s, z*z*C + c
	);
}

inline real_t mat3::operator()(int r, int c) const {
	if (r<0 || r >= 3 || c<0 || c >= 3)
		throw std::out_of_range("coord out of range");
	return m[r][c];
}

inline void mat3::set(int r, int c, real_t v) {
	if (r<0 || r >= 3 || c<0 || c >= 3)
		throw std::out_of_range("coord out of range");
	m[r][c] = v;
}

inline vec3& mat3::row(int r) {
	if (r<0 || r >= 3) {
		throw std::out_of_range("row out of range");
	}
	return *reinterpret_cast<vec3*>(m[r]);
}

inline vec3 const& mat3::row(int r) const {
	if (r<0 || r >= 3) {
		throw std::out_of_range("row out of range");
	}
	return *reinterpret_cast<vec3 const*>(m[r]);
}

inline void mat3::setColumn(int c, vec3 const& v) {
	if (c<0 || c >= 3) {
		throw std::out_of_range("column out of range");
	}
	m[0][c] = v.x;
	m[1][c] = v.y;
	m[2][c] = v.z;
}

inline mat3 mat3::transposed() const {
	return mat3(
		m[0][0], m[1][0], m[2][0],
		m[0][1], m[1][1], m[2][1],
		m[0][2], m[1][2], m[2][2]);
}

inline real_t mat3::determinant() const
{
	// return (m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[1][0]*m[2][1]*m[0][2]) -
	//        (m[0][2]*m[1][1]*m[2][0] + m[0][1]*m[1][0]*m[2][2] + m[1][2]*m[2][1]*m[0][0]);
	return  m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
		m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
}

inline mat3 mat3::inversed() const
{
	real_t const det = determinant();
	if (almostEqual(det, real_t(0.0), _EPSILON))
		return mat3(); // undefined behavier
	real_t const rdet = real_t(1.0 / det);
	/*
	mat3 tmp = this->transposed();
	auto const& m = tmp.m;
	mat3 ans;

	ans.m[0][0] =  rdet * ( m[1][1]*m[2][2] - m[1][2]*m[2][1] );
	ans.m[0][1] = -rdet * ( m[1][0]*m[2][2] - m[1][2]*m[2][0] );
	ans.m[0][2] =  rdet * ( m[1][0]*m[2][1] - m[1][1]*m[2][0] );

	ans.m[1][0] = -rdet * ( m[0][1]*m[2][2] - m[0][2]*m[2][1] );
	ans.m[1][1] =  rdet * ( m[0][0]*m[2][2] - m[0][2]*m[2][0] );
	ans.m[1][2] = -rdet * ( m[0][0]*m[2][1] - m[0][1]*m[2][0] );

	ans.m[2][0] =  rdet * ( m[0][1]*m[1][2] - m[0][2]*m[1][1] );
	ans.m[2][1] = -rdet * ( m[0][0]*m[1][2] - m[0][2]*m[2][0] );
	ans.m[2][2] =  rdet * ( m[0][0]*m[1][1] - m[0][1]*m[1][0] );
	*/

	// swaping row & col, we got:

	mat3 ans;

	ans.m[0][0] = rdet * (m[1][1] * m[2][2] - m[2][1] * m[1][2]);
	ans.m[0][1] = -rdet * (m[0][1] * m[2][2] - m[2][1] * m[0][2]);
	ans.m[0][2] = rdet * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

	ans.m[1][0] = -rdet * (m[1][0] * m[2][2] - m[2][0] * m[1][2]);
	ans.m[1][1] = rdet * (m[0][0] * m[2][2] - m[2][0] * m[0][2]);
	ans.m[1][2] = -rdet * (m[0][0] * m[1][2] - m[1][0] * m[0][2]);

	ans.m[2][0] = rdet * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
	ans.m[2][1] = -rdet * (m[0][0] * m[2][1] - m[2][0] * m[0][2]);
	ans.m[2][2] = rdet * (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

	return ans;
}

inline mat3 mat3::operator*(mat3 const& mat) const
{
	auto const& n = mat.m;
	return mat3(
		m[0][0] * n[0][0] + m[0][1] * n[1][0] + m[0][2] * n[2][0], m[0][0] * n[0][1] + m[0][1] * n[1][1] + m[0][2] * n[2][1], m[0][0] * n[0][2] + m[0][1] * n[1][2] + m[0][2] * n[2][2],
		m[1][0] * n[0][0] + m[1][1] * n[1][0] + m[1][2] * n[2][0], m[1][0] * n[0][1] + m[1][1] * n[1][1] + m[1][2] * n[2][1], m[1][0] * n[0][2] + m[1][1] * n[1][2] + m[1][2] * n[2][2],
		m[2][0] * n[0][0] + m[2][1] * n[1][0] + m[2][2] * n[2][0], m[2][0] * n[0][1] + m[2][1] * n[1][1] + m[2][2] * n[2][1], m[2][0] * n[0][2] + m[2][1] * n[1][2] + m[2][2] * n[2][2]);
}

// ref: http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
inline void mat3::setQuat(quat const& q)
{
	real_t w = q.w, x = q.x, y = q.y, z = q.z;
	real_t n = q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
	real_t s = equal(n, 0.f) ? 0.f : 2.f / n;
	real_t wx = s * w * x, wy = s * w * y, wz = s * w * z;
	real_t xx = s * x * x, xy = s * x * y, xz = s * x * z;
	real_t yy = s * y * y, yz = s * y * z, zz = s * z * z;

	*this = mat3(1 - (yy + zz), xy - wz, xz + wy,
		xy + wz, 1 - (xx + zz), yz - wx,
		xz - wy, yz + wx, 1 - (xx + yy));
}

inline bool mat3::operator==(mat3 const& rhs) const {
	return  equal(m[0][0], rhs.m[0][0]) &&
		equal(m[0][1], rhs.m[0][1]) &&
		equal(m[0][2], rhs.m[0][2]) &&
		equal(m[1][0], rhs.m[1][0]) &&
		equal(m[1][1], rhs.m[1][1]) &&
		equal(m[1][2], rhs.m[1][2]) &&
		equal(m[2][0], rhs.m[2][0]) &&
		equal(m[2][1], rhs.m[2][1]) &&
		equal(m[2][2], rhs.m[2][2]);
}

// ref http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
// ref http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
inline quat mat3::toQuat() const
{
	if (!equal(row(0).lengthSquared(), 1.f) ||
		!equal(row(1).lengthSquared(), 1.f) ||
		!equal(row(2).lengthSquared(), 1.f))
		throw std::domain_error("the matrix contains something other than rotation");
	real_t tr = trace();
	quat q;
	if (tr > 0) {
		real_t s = std::sqrt(real_t(tr + 1.0)) * 2; // s=4*qw
		q.w = real_t(0.25 * s);
		q.x = (m[2][1] - m[1][2]) / s;
		q.y = (m[0][2] - m[2][0]) / s;
		q.z = (m[1][0] - m[0][1]) / s;
	}
	else if ((m[0][0] > m[1][1])&(m[0][0] > m[2][2])) {
		real_t s = sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2; // s=4*qx
		q.w = (m[2][1] - m[1][2]) / s;
		q.x = real_t(0.25 * s);
		q.y = (m[0][1] + m[1][0]) / s;
		q.z = (m[0][2] + m[2][0]) / s;
	}
	else if (m[1][1] > m[2][2]) {
		real_t s = sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2; // s=4*qy
		q.w = (m[0][2] - m[2][0]) / s;
		q.x = (m[0][1] + m[1][0]) / s;
		q.y = real_t(0.25 * s);
		q.z = (m[1][2] + m[2][1]) / s;
	}
	else {
		real_t s = sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2; // s=4*qz
		q.w = (m[1][0] - m[0][1]) / s;
		q.x = (m[0][2] + m[2][0]) / s;
		q.y = (m[1][2] + m[2][1]) / s;
		q.z = real_t(0.25 * s);
	}
	return q;
}
// }}}
// }}}

// ---------------------------------------------------------------------------

// mat4 {{{
struct mat4 {
	real_t m[4][4];

	mat4() { *this = identity(); }
	mat4(real_t d[16]) { memcpy(m, d, 16 * sizeof(real_t)); }
	mat4(mat4 const& m) { memcpy(this->m, m.m, 16 * sizeof(real_t)); }
	mat4(mat3 const& m) {
		*this = identity();
		for (int r = 0; r<3; ++r) for (int c = 0; c<3; ++c)
			this->m[r][c] = m(r, c);
	}
	mat4(vec4 const& row0, vec4 const& row1, vec4 const& row2, vec4 const& row3) {
		row(0) = row0;
		row(1) = row1;
		row(2) = row2;
		row(3) = row3;
	}
	mat4(real_t _00, real_t _01, real_t _02, real_t _03,
		real_t _10, real_t _11, real_t _12, real_t _13,
		real_t _20, real_t _21, real_t _22, real_t _23,
		real_t _30, real_t _31, real_t _32, real_t _33) {
		m[0][0] = _00; m[0][1] = _01; m[0][2] = _02; m[0][3] = _03;
		m[1][0] = _10; m[1][1] = _11; m[1][2] = _12; m[1][3] = _13;
		m[2][0] = _20; m[2][1] = _21; m[2][2] = _22; m[2][3] = _23;
		m[3][0] = _30; m[3][1] = _31; m[3][2] = _32; m[3][3] = _33;
	}
	static mat4 identity() {
		return mat4(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}
	static mat4 eye(real_t v) {
		return mat4(v, 0.f, 0.f, 0.f,
			0.f, v, 0.f, 0.f,
			0.f, 0.f, v, 0.f,
			0.f, 0.f, 0.f, v);
	}
	static mat4 eye(vec4 const& v) {
		return mat4(v.x, 0.f, 0.f, 0.f,
			0.f, v.y, 0.f, 0.f,
			0.f, 0.f, v.z, 0.f,
			0.f, 0.f, 0.f, v.w);
	}
	static mat4 zero() {
		return mat4(0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f);
	}
	static inline mat4 fromQuat(quat const& q);
	static inline mat4 fromAxisAngle(vec3 const& axis, real_t angle);

	inline real_t      operator()(int r, int c) const;
	inline void        set(int r, int c, real_t v);
	inline vec4&       row(int r);
	inline vec4 const& row(int r) const;
	inline void        setRow(int r, vec4 const& v) { row(r) = v; }
	inline void        setColumn(int c, vec4 const& v);
	inline mat4        transposed() const;
	inline void        transpose() { *this = transposed(); }
	inline real_t      trace() { return m[0][0] + m[1][1] + m[2][2] + m[3][3]; }

	inline real_t      determinant() const;
	inline mat4        inversed() const;
	inline void        inverse() { *this = inversed(); }
	inline mat4        operator*(mat4 const& m) const;
	inline vec4        operator*(vec4 const& v) const;
	inline void        operator*=(mat4 const& m) { *this = this->operator*(m); }
	inline bool        operator==(mat4 const& m) const;
	inline bool        operator!=(mat4 const& m) const { return !this->operator==(m); }
	inline void        setQuat(quat const& q) { *this = mat4(mat3::fromQuat(q)); }

	static inline mat4 orthoLH(real_t w, real_t h, real_t zn, real_t zf);
	static inline mat4 orthoOffCenterLH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf);
	static inline mat4 lookAtLH(vec3 const& eye, vec3 const& at, vec3 const& up);
	static inline mat4 perspectiveFovLH(real_t fovy, real_t aspect, real_t zn, real_t zf);
	static inline mat4 perspectiveLH(real_t w, real_t h, real_t zn, real_t zf);
	static inline mat4 perspectiveOffCenterLH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf);

	static inline mat4 orthoRH(real_t w, real_t h, real_t zn, real_t zf);
	static inline mat4 orthoOffCenterRH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf);
	static inline mat4 lookAtRH(vec3 const& eye, vec3 const& at, vec3 const& up);
	static inline mat4 perspectiveFovRH(real_t fovy, real_t aspect, real_t zn, real_t zf);
	static inline mat4 perspectiveRH(real_t w, real_t h, real_t zn, real_t zf);
	static inline mat4 perspectiveOffCenterRH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf);

	static inline mat4 scaling(real_t x, real_t y, real_t z) { return scaling(vec3(x, y, z)); }
	static inline mat4 scaling(vec3 const& s);
	static inline mat4 rotation(real_t angle, real_t ax, real_t ay, real_t az) { return rotation(vec3(ax, ay, az), angle); }
	static inline mat4 rotation(vec3 const& axis, real_t angle);
	static inline mat4 rotation(quat const& q);
	static inline mat4 translation(real_t x, real_t y, real_t z) { return translation(vec3(x, y, z)); }
	static inline mat4 translation(vec3 const& t);
	static inline mat4 reflection(plane const& p);

	// decompose scale->rotate->translate
	inline void decompose(vec3& scale, mat4& rotate, vec3& translate) const;
	// decompose rotate->scale->translate
	// we should not do this ...
	// inline void decompose(mat4& rotate, vec3& scale, vec3& translate) const;

	inline quat toQuat() const;
};

// impl {{{
inline mat4 mat4::fromQuat(quat const& q) {
	mat4 m;
	m.setQuat(q);
	return m;
}

inline mat4 mat4::fromAxisAngle(vec3 const& axis, real_t angle) {
	return mat4(mat3::fromAxisAngle(axis, angle));
}

inline real_t mat4::operator()(int r, int c) const {
	if (r<0 || r >= 4 || c<0 || c >= 4)
		throw std::out_of_range("coord out of range");
	return m[r][c];
}

inline void mat4::set(int r, int c, real_t v) {
	if (r<0 || r >= 4 || c<0 || c >= 4)
		throw std::out_of_range("coord out of range");
	m[r][c] = v;
}

inline vec4& mat4::row(int r) {
	if (r<0 || r >= 4) {
		throw std::out_of_range("row out of range");
	}
	return *reinterpret_cast<vec4*>(m[r]);
}

inline vec4 const& mat4::row(int r) const {
	if (r<0 || r >= 4) {
		throw std::out_of_range("row out of range");
	}
	return *reinterpret_cast<vec4 const*>(m[r]);
}

inline void mat4::setColumn(int c, vec4 const& v) {
	if (c<0 || c >= 4) {
		throw std::out_of_range("column out of range");
	}
	m[0][c] = v.x;
	m[1][c] = v.y;
	m[2][c] = v.z;
	m[3][c] = v.w;
}

inline mat4 mat4::transposed() const {
	return mat4(
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]);
}


inline real_t mat4::determinant() const
{
	return m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
		m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
		m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]))
		- m[1][0] * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]))
		+ m[2][0] * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
			m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]))
		- m[3][0] * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
			m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
			m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
}

inline mat4 mat4::inversed() const
{
	real_t const det = determinant();
	if (almostEqual(det, real_t(0.0), _EPSILON))
		return mat4(); // undefined behavier
	real_t const rdet = real_t(1.0 / det);

	mat4 const tmp = this->transposed();
	auto const& m = tmp.m;
	mat4 ans;

	ans.m[0][0] = rdet * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
		m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
		m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));

	ans.m[0][1] = -rdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
		m[2][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
		m[3][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));

	ans.m[0][2] = rdet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
		m[2][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
		m[3][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]));

	ans.m[0][3] = -rdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
		m[2][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +
		m[3][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));

	ans.m[1][0] = -rdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
		m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
		m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));

	ans.m[1][1] = rdet * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
		m[2][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
		m[3][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));

	ans.m[1][2] = -rdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
		m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
		m[3][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]));

	ans.m[1][3] = rdet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
		m[2][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
		m[3][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]));

	ans.m[2][0] = rdet * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
		m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
		m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

	ans.m[2][1] = -rdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
		m[1][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
		m[3][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

	ans.m[2][2] = rdet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) -
		m[1][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
		m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));

	ans.m[2][3] = -rdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -
		m[1][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
		m[3][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));

	ans.m[3][0] = -rdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
		m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
		m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

	ans.m[3][1] = rdet * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
		m[1][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
		m[2][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

	ans.m[3][2] = -rdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -
		m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) +
		m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));

	ans.m[3][3] = rdet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
		m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));

	return ans;
}

inline mat4 mat4::operator*(mat4 const& mat) const
{
	auto const& n = mat.m;
	mat4 ans;

	ans.m[0][0] = m[0][0] * n[0][0] + m[0][1] * n[1][0] + m[0][2] * n[2][0] + m[0][3] * n[3][0];
	ans.m[0][1] = m[0][0] * n[0][1] + m[0][1] * n[1][1] + m[0][2] * n[2][1] + m[0][3] * n[3][1];
	ans.m[0][2] = m[0][0] * n[0][2] + m[0][1] * n[1][2] + m[0][2] * n[2][2] + m[0][3] * n[3][2];
	ans.m[0][3] = m[0][0] * n[0][3] + m[0][1] * n[1][3] + m[0][2] * n[2][3] + m[0][3] * n[3][3];

	ans.m[1][0] = m[1][0] * n[0][0] + m[1][1] * n[1][0] + m[1][2] * n[2][0] + m[1][3] * n[3][0];
	ans.m[1][1] = m[1][0] * n[0][1] + m[1][1] * n[1][1] + m[1][2] * n[2][1] + m[1][3] * n[3][1];
	ans.m[1][2] = m[1][0] * n[0][2] + m[1][1] * n[1][2] + m[1][2] * n[2][2] + m[1][3] * n[3][2];
	ans.m[1][3] = m[1][0] * n[0][3] + m[1][1] * n[1][3] + m[1][2] * n[2][3] + m[1][3] * n[3][3];

	ans.m[2][0] = m[2][0] * n[0][0] + m[2][1] * n[1][0] + m[2][2] * n[2][0] + m[2][3] * n[3][0];
	ans.m[2][1] = m[2][0] * n[0][1] + m[2][1] * n[1][1] + m[2][2] * n[2][1] + m[2][3] * n[3][1];
	ans.m[2][2] = m[2][0] * n[0][2] + m[2][1] * n[1][2] + m[2][2] * n[2][2] + m[2][3] * n[3][2];
	ans.m[2][3] = m[2][0] * n[0][3] + m[2][1] * n[1][3] + m[2][2] * n[2][3] + m[2][3] * n[3][3];

	ans.m[3][0] = m[3][0] * n[0][0] + m[3][1] * n[1][0] + m[3][2] * n[2][0] + m[3][3] * n[3][0];
	ans.m[3][1] = m[3][0] * n[0][1] + m[3][1] * n[1][1] + m[3][2] * n[2][1] + m[3][3] * n[3][1];
	ans.m[3][2] = m[3][0] * n[0][2] + m[3][1] * n[1][2] + m[3][2] * n[2][2] + m[3][3] * n[3][2];
	ans.m[3][3] = m[3][0] * n[0][3] + m[3][1] * n[1][3] + m[3][2] * n[2][3] + m[3][3] * n[3][3];

	return ans;
}

inline bool mat4::operator==(mat4 const& rhs) const {
	return  equal(m[0][0], rhs.m[0][0]) &&
		equal(m[0][1], rhs.m[0][1]) &&
		equal(m[0][2], rhs.m[0][2]) &&
		equal(m[0][3], rhs.m[0][3]) &&
		equal(m[1][0], rhs.m[1][0]) &&
		equal(m[1][1], rhs.m[1][1]) &&
		equal(m[1][2], rhs.m[1][2]) &&
		equal(m[1][3], rhs.m[1][3]) &&
		equal(m[2][0], rhs.m[2][0]) &&
		equal(m[2][1], rhs.m[2][1]) &&
		equal(m[2][2], rhs.m[2][2]) &&
		equal(m[2][3], rhs.m[2][3]) &&
		equal(m[3][0], rhs.m[3][0]) &&
		equal(m[3][1], rhs.m[3][1]) &&
		equal(m[3][2], rhs.m[3][2]) &&
		equal(m[3][3], rhs.m[3][3]);
}

inline quat mat4::toQuat() const
{
	if (vec4(0.f, 0.f, 0.f, 1.f) != row(3) ||
		vec4(0.f, 0.f, 0.f, 1.f) != transposed().row(3)) {
		throw std::domain_error("the matrix seems not to be a rotation matrix");
	}
	return mat3(m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2]).toQuat();
}
// }}}
// }}}

// ---------------------------------------------------------------------------

inline mat3 mult(mat3 const& m1, mat3 const& m2)
{
	return m1*m2;
}

inline mat4 mult(mat4 const& m1, mat4 const& m2)
{
	return m1*m2;
}

inline vec3 mult(mat3 const& m, vec3 const& v)
{
	// v as column vector
	return vec3(
		dot(m.row(0), v),
		dot(m.row(1), v),
		dot(m.row(2), v)
	);
}

inline vec3 mult(vec3 const& v, mat3 const& mt)
{
	// v as row vector
	mat3 m = mt.transposed();
	return vec3(
		dot(m.row(0), v),
		dot(m.row(1), v),
		dot(m.row(2), v)
	);
}

inline vec4 mult(mat4 const& m, vec4 const& v)
{
	// v as column vector
	return vec4(
		dot(m.row(0), v),
		dot(m.row(1), v),
		dot(m.row(2), v),
		dot(m.row(3), v)
	);
}

inline vec4 mult(vec4 const& v, mat4 const& mt)
{
	// v as row vector
	mat4 m = mt.transposed();
	return vec4(
		dot(m.row(0), v),
		dot(m.row(1), v),
		dot(m.row(2), v),
		dot(m.row(3), v)
	);
}

inline real_t det(mat3 const& m)
{
	return m.determinant();
}

inline real_t det(mat4 const& m)
{
	return m.determinant();
}

inline mat3 inv(mat3 const& m)
{
	return m.inversed();
}

inline mat4 inv(mat4 const& m)
{
	return m.inversed();
}

inline quat quat::fromMat(mat3 const& m)
{
	return m.toQuat();
}

inline quat quat::fromMat(mat4 const& m)
{
	return m.toQuat();
}

inline vec3 vec3::operator*(mat3 const& m) const { return mult(*this, m); }

inline vec4 vec4::operator*(mat4 const& m) const { return mult(*this, m); }

inline vec3 mat3::operator*(vec3 const& v) const { return mult(*this, v); }

inline vec4 mat4::operator*(vec4 const& v) const { return mult(*this, v); }

inline bool equal(mat3 const& m1, mat3 const& m2) { return m1 == m2; }

inline bool equal(mat4 const& m1, mat4 const& m2) { return m1 == m2; }
// }}}

// ---------------------------------------------------------------------------

// plane {{{
// ref http://mathworld.wolfram.com/Plane.html
struct plane {
	real_t a, b, c, d; // ax+by+cz+d = 0
					   // where (a,b,c) is the normal vector and d is distance to
					   // the origin point
	plane() : a(0.f), b(0.f), c(0.f), d(0.f) {}
	plane(real_t a, real_t b, real_t c, real_t d) : a(a), b(b), c(c), d(d) {}
	plane(vec3 const& v, real_t d) : a(v.x), b(v.y), c(v.z), d(d) {}
	static inline plane fromPointNormal(vec3 const& pt, vec3 nm);
	static inline plane fromPoints(vec3 const& p1, vec3 const& p2, vec3 const& p3);

	inline plane  normalized() const;
	inline void   normalize() { *this = normalized(); }

	inline real_t distance(vec3 const& p) const;
	inline int    side(vec3 const& p) const;
	inline plane  fliped() const { return plane(-a, -b, -c, d); }
	inline void   flip() { *this = fliped(); }
	inline vec3   reflect(vec3 const& p) const;

	inline bool   operator==(plane const& other) const;
	inline bool   operator!=(plane const& other) const { return !this->operator==(other); }
};

// impl {{{
inline plane plane::normalized() const
{
	vec3 norm = vec3(a, b, c).normalized();
	return plane(norm, d);
}

inline real_t plane::distance(vec3 const& p) const
{
	return dot(p, vec3(a, b, c)) + d;
}

inline int plane::side(vec3 const& p) const
{
	real_t dist = distance(p);
	if (dist < -_EPSILON)
		return -1;
	else if (dist > _EPSILON)
		return 1;
	else
		return 0;
}

inline plane plane::fromPointNormal(vec3 const& pt, vec3 nm) {
	nm.normalize();
	return plane(nm, -dot(nm, pt));
}

inline plane plane::fromPoints(vec3 const& p1, vec3 const& p2, vec3 const& p3) {
	vec3 v1 = p2 - p1;
	vec3 v2 = p3 - p1;
	vec3 norm = cross(v1, v2).normalized();
	return plane(norm, -dot(norm, p1));
}

inline vec3 plane::reflect(vec3 const& p) const {
	real_t const diff = -2 * distance(p);
	return vec3(p.x + a*diff, p.y + b*diff, p.z + c*diff);
}

inline bool plane::operator==(plane const& other) const {
	return equal(a, other.a) && equal(b, other.b) && equal(c, other.c) && equal(d, other.d);
}
// }}}
// }}}

// ---------------------------------------------------------------------------

// utils {{{

inline bool valid(real_t v)
{
#ifndef _MSC_VER
	return std::isfinite(v) && !std::isnan(v);
#else
	return _finite(v) && !_isnan(v);
#endif
}

inline bool valid(vec2 const& v)
{
	return valid(v.x) && valid(v.y);
}

inline bool valid(vec3 const& v)
{
	return valid(v.x) && valid(v.y) && valid(v.z);
}

inline bool valid(vec4 const& v)
{
	return valid(v.x) && valid(v.y) && valid(v.z) && valid(v.w);
}

inline bool valid(quat const& v)
{
	return valid(v.x) && valid(v.y) && valid(v.z) && valid(v.w);
}

inline bool valid(plane const& p)
{
	return valid(p.a) && valid(p.b) && valid(p.c) && valid(p.d);
}

inline bool valid(mat3 const& m)
{
	for (int r = 0; r<3; ++r)
		for (int c = 0; c<3; ++c)
			if (!valid(m.m[r][c]))
				return false;
	return true;
}

inline bool valid(mat4 const& m)
{
	for (int r = 0; r<4; ++r)
		for (int c = 0; c<4; ++c)
			if (!valid(m.m[r][c]))
				return false;
	return true;
}

inline real_t clamp(real_t v, real_t min, real_t max)
{
	return (std::max)((std::min)(v, max), min);
}

inline vec2   clamp(vec2 const& v, vec2 const& min, vec2 const& max)
{
	return vec2(clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y));
}

inline vec3   clamp(vec3 const& v, vec3 const& min, vec3 const& max)
{
	return vec3(
		clamp(v.x, min.x, max.x),
		clamp(v.y, min.y, max.y),
		clamp(v.z, min.z, max.z));
}

inline vec4   clamp(vec4 const& v, vec4 const& min, vec4 const& max)
{
	return vec4(
		clamp(v.x, min.x, max.x),
		clamp(v.y, min.y, max.y),
		clamp(v.z, min.z, max.z),
		clamp(v.w, min.w, max.w));
}
// }}}

// ---------------------------------------------------------------------------

// 3d stuff {{{
inline mat4 mat4::orthoLH(real_t w, real_t h, real_t zn, real_t zf) {
	return mat4(2.f / w, 0.f, 0.f, 0.f,
		0.f, 2.f / h, 0.f, 0.f,
		0.f, 0.f, 1.f / (zf - zn), 0.f,
		0.f, 0.f, -zn / (zf - zn), 1.f);
}
inline mat4 mat4::orthoOffCenterLH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf) {
	return mat4(2.f / (right - left), 0.f, 0.f, 0.f,
		0.f, 2.f / (top - bottom), 0.f, 0.f,
		0.f, 0.f, 1.f / (zf - zn), 0.f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), zn / (zn - zf), 1.f);
}
inline mat4 mat4::lookAtLH(vec3 const& eye, vec3 const& at, vec3 const& up) {
	vec3 const z = (at - eye).normalized();
	vec3 const x = cross(up, z).normalized();
	vec3 const y = cross(z, x);

	return mat4(x.x, y.x, z.x, 0.f,
		x.y, y.y, z.y, 0.f,
		x.z, y.z, z.z, 0.f,
		-dot(x, eye), -dot(y, eye), -dot(z, eye), 1.f);
}
inline mat4 mat4::perspectiveFovLH(real_t fovy, real_t aspect, real_t zn, real_t zf) {
	real_t const ys = std::tan(real_t(3.1415926535897932384626433832795 / 2.0) - fovy / 2.f); // cot(fovy / 2.f)
	real_t const xs = ys / aspect;

	return mat4(xs, 0.f, 0.f, 0.f,
		0.f, ys, 0.f, 0.f,
		0.f, 0.f, zf / (zf - zn), 1.f,
		0.f, 0.f, -zn*zf / (zf - zn), 0.f);
}
inline mat4 mat4::perspectiveLH(real_t w, real_t h, real_t zn, real_t zf) {
	return mat4(2.f*zn / w, 0.f, 0.f, 0.f,
		0.f, 2.f*zn / h, 0.f, 0.f,
		0.f, 0.f, zf / (zf - zn), 1.f,
		0.f, 0.f, zn*zf / (zn - zf), 0.f);
}
inline mat4 mat4::perspectiveOffCenterLH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf) {
	return mat4(2.f*zn / (right - left), 0.f, 0.f, 0.f,
		0.f, 2.f*zn / (top - bottom), 0.f, 0.f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), zf / (zf - zn), 1.f,
		0.f, 0.f, zn*zf / (zn - zf), 0.f);
}

inline mat4 mat4::orthoRH(real_t w, real_t h, real_t zn, real_t zf) {
	return mat4(2.f / w, 0.f, 0.f, 0.f,
		0.f, 2.f / h, 0.f, 0.f,
		0.f, 0.f, 1.f / (zn - zf), 0.f,
		0.f, 0.f, zn / (zn - zf), 1.f);
}
inline mat4 mat4::orthoOffCenterRH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf) {
	return mat4(2.f / (right - left), 0.f, 0.f, 0.f,
		0.f, 2.f / (top - bottom), 0.f, 0.f,
		0.f, 0.f, 1.f / (zn - zf), 0.f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), zn / (zn - zf), 1.f);
}
inline mat4 mat4::lookAtRH(vec3 const& eye, vec3 const& at, vec3 const& up) {
	return lookAtLH(eye, eye * 2 - at, up);
}
inline mat4 mat4::perspectiveFovRH(real_t fovy, real_t aspect, real_t zn, real_t zf) {
	mat4 m = perspectiveFovLH(fovy, aspect, zn, zf);
	m.row(2) = -m.row(2);
	return m;
}
inline mat4 mat4::perspectiveRH(real_t w, real_t h, real_t zn, real_t zf) {
	mat4 m = perspectiveLH(w, h, zn, zf);
	m.row(2) = -m.row(2);
	return m;
}
inline mat4 mat4::perspectiveOffCenterRH(real_t left, real_t right, real_t bottom, real_t top, real_t zn, real_t zf) {
	mat4 m = perspectiveOffCenterLH(left, right, bottom, top, zn, zf);
	m.row(2) = -m.row(2);
	return m;
}

inline mat4 mat4::scaling(vec3 const& s) {
	return eye(vec4(s.x, s.y, s.z, 1.f));
}
inline mat4 mat4::rotation(vec3 const& axis, real_t angle) {
	return fromAxisAngle(axis, angle);
}
inline mat4 mat4::rotation(quat const& q) {
	return fromQuat(q);
}
inline mat4 mat4::translation(vec3 const& t) {
	return mat4(1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		t.x, t.y, t.z, 1.f);
}

// ref plane::reflect
inline mat4 mat4::reflection(plane const& p) {
	plane pl = p.normalized();
	mat4  m = mat4::identity();
	vec4  n2 = vec4(-2 * pl.a, -2 * pl.b, -2 * pl.c, 0);
	m.row(0) += n2*pl.a; // [ 1 - 2 a a,    -2 b a,    -2 c a,   0 ]
	m.row(1) += n2*pl.b; // [    -2 a b, 1 - 2 b b,    -2 c b,   0 ]
	m.row(2) += n2*pl.c; // [    -2 a c,    -2 b c, 1 - 2 c c,   0 ]
	m.row(3) += n2*pl.d; // [    -2 a d,    -2 b d,    -2 c d,   1 ] 
	return m;
}

inline void mat4::decompose(vec3& scale, mat4& rotate, vec3& translate) const {
	if (!(equal(m[0][3], 0.f) &&
		equal(m[1][3], 0.f) &&
		equal(m[2][3], 0.f) &&
		equal(m[3][3], 1.f)))
		throw std::domain_error("last column of the matrix should be (0,0,0,1)");
	scale.x = row(0).length();
	scale.y = row(1).length();
	scale.z = row(2).length();
	rotate = mat4(row(0) / scale.x, row(1) / scale.y, row(2) / scale.z, vec4(0.f, 0.f, 0.f, 1.f));
	translate = row(3).xyz();
}

// we should not do this ...
/*
inline void mat4::decompose(mat4& rotate, vec3& scale, vec3& translate) const {
translate = row(3).xyz();
mat4 xt = mat4(row(0),row(1),row(2),vec4(0.f,0.f,0.f,1.f)).transposed(); // without translation and transposed
scale = vec3(xt.row(0).length(), xt.row(1).length(), xt.row(2).length());
xt.row(0) /= scale.x;
xt.row(1) /= scale.y;
xt.row(2) /= scale.z;
rotate = xt.transposed();
}
*/
// }}}

// TODO: color(?) / eigenvalue(?) / frustum(?)
// vi:foldmethod=marker foldmarker={{{,}}}
