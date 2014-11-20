#ifndef COLLISION2D_H
#define COLLISION2D_H

#include "Objects2d.h"

namespace physics {

	inline bool rectsOverlap(const vector2d& r1Position, const vector2d& r2Position, const vector2d& r1Extents, const vector2d& r2Extents);

	inline bool sweepOverlap(const vector2d& d, const vector2d& aMin, const vector2d& aMax, const vector2d& bMin, const vector2d& bMax, float& normalizedTimeOfCollision);

	inline bool boxCollidesWithHorizontalLine(const Box& box, const HorizontalLine& line);

	inline bool boxCollidesWithVerticalLine(const Box& box, const VerticalLine& line);

	bool movingBoxCollidesWithHorizontalLine(float dt, const MovingBox& box, const HorizontalLine& line, vector2d& collisionPositionOfBox, float& normalizedTimeOfCollision);

	bool movingBoxCollidesWithVerticalLine(float dt, const MovingBox& box, const VerticalLine& line, vector2d& collisionPositionOfBox, float& normalizedTimeOfCollision);

	inline bool boxesCollide(const Box& a, const Box& b);

	bool movingBoxesCollide(float dt, const MovingBox& a, const MovingBox& b, vector2d& collisionPositionOfA, vector2d& collisionPositionOfB, float& normalizedTimeOfCollision);

	vector2d paddedCollisionPosition(const vector2d& positionOfA, const vector2d& extentsOfA, const vector2d& positionOfB, const vector2d& extentsOfB);
}

#endif