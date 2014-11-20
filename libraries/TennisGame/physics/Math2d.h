#ifndef MATH2D_H
#define MATH2D_H

namespace physics {

	#define VECTOR2D_ZERO vector2d(0, 0)

	struct vector2d {
		float x;
		float y;

		vector2d() : x(0.0f), y(0.0f) {}

		vector2d(float _x, float _y) : x(_x), y(_y) {}
	
		// Vector addition
		vector2d& operator +=(const vector2d& v) {
			x += v.x;
			y += v.y;
			return *this;
		}
	
		vector2d operator +(const vector2d& v) const {
			return vector2d(x + v.x, y + v.y);
		}

		// Vector Subtraction
		vector2d& operator -=(const vector2d& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}

		vector2d operator -(const vector2d& v) const {
			return vector2d(x - v.x, y - v.y);
		}

		// Scalar multiplication
		vector2d& operator *=(float s) {
			x *= s;
			y *= s;
			return *this;
		}

		vector2d operator *(float s) const {
			return vector2d(x * s, y * s);
		}

		// Scalar division
		vector2d& operator /=(float s) {
			x /= s;
			y /= s;
			return *this;
		}

		vector2d operator /(float s) const {
			return vector2d(x / s, y / s);
		}
	};

}

#endif