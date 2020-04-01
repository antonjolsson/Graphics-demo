#pragma once

#include <cmath>

class Vector2D {

public:
	double x;
	double y;

	Vector2D operator+(const Vector2D& rhs)
	{
		return {x + rhs.x, y + rhs.y};
	}

	Vector2D operator-(const Vector2D& rhs)
	{
		return {x - rhs.x, y - rhs.y};
	}

	Vector2D operator*(const Vector2D& rhs)
	{
		return {x * rhs.x, y * rhs.y};
	}

	Vector2D operator/(const double& rhs)
	{
		return {x / rhs, y / rhs};
	}

	Vector2D operator*(const double& rhs)
	{
		return {x * rhs, y * rhs};
	}

    Vector2D operator*(float& rhs)
    {
        return {x * rhs, y * rhs};
    }

	void operator*=(const int& rhs) {
	    x = x * rhs;
	    y = y * rhs;
	}

    void operator+=(const Vector2D& rhs) {
        x = x + rhs.x;
        y = y + rhs.y;
    }

    void operator-=(const Vector2D& rhs) {
        x = x - rhs.x;
        y = y - rhs.y;
    }

	double dotProduct(const Vector2D& rhs) {
		return x * rhs.x + y * rhs.y;
	}

    double getPointDistance(Vector2D p2) {
        return sqrt(pow(x - p2.x, 2) + pow(y - p2.y, 2));
    }

	double vectorLength() {
	    Vector2D origin {0, 0};
        return getPointDistance(origin);
	}

    Vector2D getNormal() {
        double edgeLength = vectorLength();
        return Vector2D{y, -x} / edgeLength;
    }

	Vector2D(double x, double y) {
		this->x = x;
		this->y = y;
	}
	Vector2D() {
		this->x = 0;
		this->y = 0;
	}
};