#include "point3f.hpp"
#include <sstream>

using namespace std;

string float_to_string(float value) {
    ostringstream oss;
    oss.precision(10);
    oss << std::fixed << value;
    string s = oss.str();
    s.erase(s.find_last_not_of('0') + 1, string::npos);
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}

Point3f::Point3f() : x(0), y(0), z(0) {}

Point3f::Point3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

string Point3f::to_string() const {
    return float_to_string(x) + " " + float_to_string(y) + " " + float_to_string(z);
}

Point3f Point3f::from_string(const string& s) {
    istringstream iss(s);
    float x = 0, y = 0, z = 0;
    iss >> x >> y >> z;
    return {x, y, z};
}

Point3f Point3f::operator+(const Point3f& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Point3f Point3f::operator-(const Point3f& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Point3f Point3f::operator*(float a) const {
    return {a * x, a * y, a * z};
}

float Point3f::dot(const Point3f& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Point3f Point3f::cross(const Point3f& other) const {
    return Point3f(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}
