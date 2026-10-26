#ifndef VEC3_H
#define VEC3_H

// Section 3
#include <cmath>
#include <ostream>
#include "random.h"

class vec3
{
public:
    double e[3];

    vec3() : e{0, 0, 0} {}
    vec3(double x, double y, double z) : e{x, y, z} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-x(), -y(), -z()); }
    double operator[](int index) const { return e[index]; }
    double &operator[](int index) { return e[index]; }

    vec3 &operator+=(const vec3 &other)
    {
        for (int axis = 0; axis < 3; ++axis)
            e[axis] += other[axis];
        return *this;
    }

    vec3 &operator*=(double scale)
    {
        for (double &component : e)
            component *= scale;
        return *this;
    }

    vec3 &operator/=(double scale) { return *this *= 1.0 / scale; }
    double length_squared() const { return x() * x() + y() * y() + z() * z(); }
    double length() const { return std::sqrt(length_squared()); }
    bool near_zero() const
    {
        return std::fabs(x()) < 1e-8 && std::fabs(y()) < 1e-8 && std::fabs(z()) < 1e-8;
    }
};

using point3 = vec3;

inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator+(vec3 left, const vec3 &right) { return left += right; }
inline vec3 operator-(vec3 left, const vec3 &right) { return left += -right; }
inline vec3 operator*(vec3 v, double scale) { return v *= scale; }
inline vec3 operator*(double scale, vec3 v) { return v *= scale; }
inline vec3 operator/(vec3 v, double scale) { return v /= scale; }

inline vec3 operator*(const vec3 &a, const vec3 &b)
{
    return vec3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

inline double dot(const vec3 &a, const vec3 &b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

inline vec3 cross(const vec3 &a, const vec3 &b)
{
    return vec3(a.y() * b.z() - a.z() * b.y(),
                a.z() * b.x() - a.x() * b.z(),
                a.x() * b.y() - a.y() * b.x());
}

inline vec3 unit_vector(const vec3 &v) { return v / v.length(); }

inline vec3 random_unit_vector()
{
    while (true)
    {
        const double x = 2 * random_double() - 1;
        const double y = 2 * random_double() - 1;
        const double z = 2 * random_double() - 1;
        const vec3 p(x, y, z);
        const double length_squared = p.length_squared();
        // Reject outside the sphere, and avoid dividing by an almost-zero length.
        if (1e-160 < length_squared && length_squared <= 1)
            return p / std::sqrt(length_squared);
    }
}

inline vec3 lambertian_direction(const vec3 &normal)
{
    const vec3 direction = normal + random_unit_vector();
    return direction.near_zero() ? normal : direction;
}

#endif
