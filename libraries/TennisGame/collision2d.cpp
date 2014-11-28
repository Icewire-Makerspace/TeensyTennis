#include "collision2d.h"

namespace physics {

	inline bool rectsOverlap(const vector2d& r1Position, const vector2d& r2Position, const vector2d& r1Extents, const vector2d& r2Extents) {
		// Check whether boxes overlap
		vector2d v1 = r1Position - r2Position;
		vector2d v2 = r1Extents + r2Extents;

		if (v1.x < 0) {
			v1.x *= -1.0f;
		}

		if (v1.y < 0) {
			v1.y *= -1.0f;
		}

		if (v1.x <= v2.x && v1.y <= v2.y) {
			return true;
		}

		return false;
	}

	inline bool sweepOverlap(const vector2d& d, const vector2d& aMin, const vector2d& aMax, const vector2d& bMin, const vector2d& bMax, float& normalizedTimeOfCollision) {
		if (d.x == 0.0f) {
			if ((aMin.x > bMax.x) || (aMax.x < bMin.x)) {
				return false;
			}
		}

		if (d.y == 0.0f) {
			if ((aMin.y > bMax.y) || (aMax.y < bMin.y)) {
				return false;
			}
		}

		// Normalized interval for dt
		float tStart = 0.0f;
		float tEnd = 1.0f;

		// Check for collision in x-axis
		if (d.x != 0.0f) {
			float dInverse = 1.0f / d.x;
		
			// Compute times for when they begin and end overlapping
			float xStart = (aMin.x - bMax.x) * dInverse;
			float xEnd = (aMax.x - bMin.x) * dInverse;
		
			// Check for interval out of order
			if (xStart > xEnd) {
				float t = xStart;
				xStart = xEnd;
				xEnd = t;
			}
		
			// Update interval
			if (xStart > tStart) {
				tStart = xStart;
			}

			if (xEnd < tEnd) {
				tEnd = xEnd;
			}

			// Check if this results in an empty interval
			if (tStart > tEnd) {
				return false;
			}
		}

		// Check for collision in y-axis
		if (d.y != 0.0f) {
			float dInverse = 1.0f / d.y;

			// Compute times for when they begin and end overlapping
			float yStart = (aMin.y - bMax.y) * dInverse;
			float yEnd = (aMax.y - bMin.y) * dInverse;

			// Check for interval out of order
			if (yStart > yEnd) {
				float t = yStart;
				yStart = yEnd;
				yEnd = t;
			}

			// Update interval
			if (yStart > tStart) {
				tStart = yStart;
			}

			if (yEnd < tEnd) {
				tEnd = yEnd;
			}

			// Check if this results in an empty interval
			if (tStart > tEnd) {
				return false;
			}
		}

		// We have an overlap at time
		normalizedTimeOfCollision = tStart;
		return true;
	}
	
	inline bool boxCollidesWithHorizontalLine(const Box& box, const HorizontalLine& line) {
		return rectsOverlap(box.position, line.position, box.extents, vector2d(line.extent, 0));
	}

	inline bool boxCollidesWithVerticalLine(const Box& box, const VerticalLine& line) {
		return rectsOverlap(box.position, line.position, box.extents, vector2d(0, line.extent));
	}

	bool movingBoxCollidesWithHorizontalLine(float dt, const MovingBox& box, const HorizontalLine& line, vector2d& collisionPositionOfBox, float& normalizedTimeOfCollision) {
		// Is there a collision between the objects at their current position?
		if (boxCollidesWithHorizontalLine(box, line)) {
			collisionPositionOfBox = box.position;
			normalizedTimeOfCollision = 0;
			return true;
		}

		// Displacement
		vector2d d = VECTOR2D_ZERO - ((box.position + box.velocity * dt) - box.position);

		// Quick check if the box isn't moving
		if (d.x == 0.0f && d.y == 0.0f) {
			// Since we already checked if they're currently colliding
			return false;
		}

		// Vertices
		vector2d boxMin = box.position - box.extents;
		vector2d boxMax = box.position + box.extents;
		vector2d lineMin(line.position.x - line.extent, line.position.y);
		vector2d lineMax(line.position.x + line.extent, line.position.y);

		if (sweepOverlap(d, boxMin, boxMax, lineMin, lineMax, normalizedTimeOfCollision)) {
			// We have a collision!
			collisionPositionOfBox = box.position + (d * normalizedTimeOfCollision);
			return true;
		}

		// No collision
		return false;
	}

	bool movingBoxCollidesWithVerticalLine(float dt, const MovingBox& box, const VerticalLine& line, vector2d& collisionPositionOfBox, float& normalizedTimeOfCollision) {
		// Is there a collision between the objects at their current position?
		if (boxCollidesWithVerticalLine(box, line)) {
			collisionPositionOfBox = box.position;
			normalizedTimeOfCollision = 0;
			return true;
		}

		// Displacement
		vector2d d = VECTOR2D_ZERO - ((box.position + box.velocity * dt) - box.position);

		// Quick check if the box isn't moving
		if (d.x == 0.0f && d.y == 0.0f) {
			// Since we already checked if they're currently colliding
			return false;
		}

		// Vertices
		vector2d boxMin = box.position - box.extents;
		vector2d boxMax = box.position + box.extents;
		vector2d lineMin(line.position.x, line.position.y - line.extent);
		vector2d lineMax(line.position.x, line.position.y + line.extent);

		if (sweepOverlap(d, boxMin, boxMax, lineMin, lineMax, normalizedTimeOfCollision)) {
			// We have a collision!
			collisionPositionOfBox = box.position + (d * normalizedTimeOfCollision);
			return true;
		}

		// No collision
		return false;
	}

	inline bool boxesCollide(const Box& a, const Box& b) {
		return rectsOverlap(a.position, b.position, a.extents, b.extents);
	}

	bool movingBoxesCollide(float dt, const MovingBox& a, const MovingBox& b, vector2d& collisionPositionOfA, vector2d& collisionPositionOfB, float& normalizedTimeOfCollision) {
		// TODO? broad phase collision check

		// Is there a collision between the objects at their current position?
		if (boxesCollide(a, b)) {
			collisionPositionOfA = a.position;
			collisionPositionOfB = b.position;
			normalizedTimeOfCollision = 0;
			return true;
		}

		// Displacement
		vector2d da = (a.position + a.velocity * dt) - a.position;
		vector2d db = (b.position + b.velocity * dt) - b.position;

		// Solved in A's frame of reference
		vector2d d = db - da;

		// Quick check if they're not moving or moving at the same speed in one of the axes
		if (d.x == 0.0f && d.y == 0.0f) {
			// Since we already checked if they're currently colliding
			return false;
		}

		// Vertices
		vector2d aMin = a.position - a.extents;
		vector2d aMax = a.position + a.extents;
		vector2d bMin = b.position - b.extents;
		vector2d bMax = b.position + b.extents;

		if (sweepOverlap(d, aMin, aMax, bMin, bMax, normalizedTimeOfCollision)) {
			// We have a collision!
			collisionPositionOfA = a.position + (da * normalizedTimeOfCollision);
			collisionPositionOfB = b.position + (db * normalizedTimeOfCollision);
			return true;
		}

		// No collision
		return false;
	}

	const float padding = 0.01f;
	vector2d paddedCollisionPosition(const vector2d& positionOfA, const vector2d& extentsOfA, const vector2d& positionOfB, const vector2d& extentsOfB) {

		vector2d position = positionOfA;

		float dx;
		float dy;

		float px = 0;
		float py = 0;

		float dx1 = positionOfA.x + extentsOfA.x - positionOfB.x + extentsOfB.x;
		if (dx1 < 0) {
			dx1 = -dx1;
		}

		float dx2 = positionOfB.x + extentsOfB.x - positionOfA.x + extentsOfA.x;
		if (dx2 < 0) {
			dx2 = -dx2;
		}

		if (dx1 <= dx2) {
			dx = dx1;
			px = positionOfB.x - extentsOfB.x - extentsOfA.x - padding;
		} else {
			dx = dx2;
			px = positionOfB.x + extentsOfB.x + extentsOfA.x + padding;
		}

		float dy1 = positionOfA.y + extentsOfA.y - positionOfB.y + extentsOfB.y;
		if (dy1 < 0) {
			dy1 = -dy1;
		}

		float dy2 = positionOfB.y + extentsOfB.y - positionOfA.y + extentsOfA.y;
		if (dy2 < 0) {
			dy2 = -dy2;
		}

		if (dy1 <= dy2) {
			dy = dy1;
			py = positionOfB.y - extentsOfB.y - extentsOfA.y - padding;
		} else {
			dy = dy2;
			py = positionOfB.y + extentsOfB.y + extentsOfA.y + padding;
		}

		if (dx < dy) {
			position.x = px;
		} else if (dy < dx) {
			position.y = py;
		} else {
			position.x = px;
			position.y = py;
		}

		return position;
	}
}