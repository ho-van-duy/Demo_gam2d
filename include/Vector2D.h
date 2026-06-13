#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <iostream>

class Vector2D {
public:
    float x, y;

    Vector2D() : x(0.0f), y(0.0f) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    // Arithmetic operators
    Vector2D operator+(const Vector2D& other) const { return Vector2D(x + other.x, y + other.y); }
    Vector2D operator-(const Vector2D& other) const { return Vector2D(x - other.x, y - other.y); }
    Vector2D operator*(float scalar) const { return Vector2D(x * scalar, y * scalar); }
    Vector2D operator/(float scalar) const { return Vector2D(x / scalar, y / scalar); }

    Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
    Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2D& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    bool operator==(const Vector2D& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2D& other) const { return !(*this == other); }

    // Vector operations
    float magnitude() const { return std::sqrt(x * x + y * y); }
    float magnitudeSquared() const { return x * x + y * y; }

    Vector2D normalized() const {
        float mag = magnitude();
        if (mag > 0.0001f) return Vector2D(x / mag, y / mag);
        return Vector2D(0, 0);
    }

    void normalize() {
        float mag = magnitude();
        if (mag > 0.0001f) { x /= mag; y /= mag; }
    }

    float dot(const Vector2D& other) const { return x * other.x + y * other.y; }
    float cross(const Vector2D& other) const { return x * other.y - y * other.x; }

    float distanceTo(const Vector2D& other) const {
        return (*this - other).magnitude();
    }

    Vector2D lerp(const Vector2D& target, float t) const {
        return *this + (target - *this) * t;
    }

    static Vector2D Zero()  { return Vector2D(0, 0); }
    static Vector2D One()   { return Vector2D(1, 1); }
    static Vector2D Up()    { return Vector2D(0, -1); }
    static Vector2D Down()  { return Vector2D(0, 1); }
    static Vector2D Left()  { return Vector2D(-1, 0); }
    static Vector2D Right() { return Vector2D(1, 0); }

    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};

#endif // VECTOR2D_H
