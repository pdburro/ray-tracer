#ifndef VEC3_H
#define VEC3_H

// Section 3 of Ray Tracing in One Weekend:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#thevec3class
#include <cmath>
#include <ostream>

class vec3 {
public:
    double e[3];

    vec3() : e{0, 0, 0} {}
    vec3(double x, double y, double z) : e{x, y, z} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-x(), -y(), -z()); }
    double operator[](int index) const { return e[index]; }
    double& operator[](int index) { return e[index]; }

    vec3& operator+=(const vec3& other) {
        for (int axis = 0; axis < 3; ++axis) e[axis] += other[axis];
        return *this;
    }

    vec3& operator*=(double scale) {
        for (double& component : e) component *= scale;
        return *this;
    }

    vec3& operator/=(double scale) { return *this *= 1.0 / scale; }
    double length_squared() const { return x()*x() + y()*y() + z()*z(); }
    double length() const { return std::sqrt(length_squared()); }
};

// A position uses the same storage and operations as a vector.
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator+(vec3 left, const vec3& right) { return left += right; }
inline vec3 operator-(vec3 left, const vec3& right) { return left += -right; }
inline vec3 operator*(vec3 v, double scale) { return v *= scale; }
inline vec3 operator*(double scale, vec3 v) { return v *= scale; }
inline vec3 operator/(vec3 v, double scale) { return v /= scale; }

inline vec3 operator*(const vec3& a, const vec3& b) {
    return vec3(a.x()*b.x(), a.y()*b.y(), a.z()*b.z());
}

inline double dot(const vec3& a, const vec3& b) {
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y()*b.z() - a.z()*b.y(),
                a.z()*b.x() - a.x()*b.z(),
                a.x()*b.y() - a.y()*b.x());
}

// The input must have nonzero length.
inline vec3 unit_vector(const vec3& v) { return v / v.length(); }

#endif
