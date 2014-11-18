#ifndef WALL_H
#define WALL_H

#include "physics\objects2d.h"

class HorizontalWall {
public:
	void setup(physics::vector2d p, float length);
	physics::vector2d getPosition() const;
	float getExtent() const;
	physics::vector2d getExtents() const;
	const physics::HorizontalLine* getPhysicsObject() const;

private:
	physics::HorizontalLine line;
};

class VerticalWall {
public:
	void setup(physics::vector2d p, float height);
	physics::vector2d getPosition() const;
	float getExtent() const;
	physics::vector2d getExtents() const;
	const physics::VerticalLine* getPhysicsObject() const;

private:
	physics::VerticalLine line;
};

#endif