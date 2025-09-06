#pragma once
#include <string>

using namespace std;

string float_to_string(float value);

struct Point3f {
    float x, y, z;
    Point3f();
    Point3f(float x_, float y_, float z_);

    [[nodiscard]] string to_string() const;

    static Point3f from_string(const string& s);

    Point3f operator+(const Point3f& other) const;
    Point3f operator-(const Point3f& other) const;
    Point3f operator*(float a) const;
    [[nodiscard]] float dot(const Point3f& other) const;
    [[nodiscard]] Point3f cross(const Point3f& other) const;
};
