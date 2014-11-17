#include "wall.h"

void HorizontalWall::setup(physics::vector2d p, float length) {
	line.extent = length * 0.5f;
	line.position.x = p.x + line.extent;
	line.position.y = p.y;
}

physics::vector2d HorizontalWall::getPosition() const {
	return line.position;
}

float HorizontalWall::getExtent() const {
	return line.extent;
}

physics::vector2d HorizontalWall::getExtents() const {
	return physics::vector2d(line.extent, 0.0f);
}

const physics::HorizontalLine* HorizontalWall::getPhysicsObject() const {
	return &line;
}

void VerticalWall::setup(physics::vector2d p, float height) {
	line.extent = height * 0.5f;
	line.position.x = p.x;
	line.position.y = p.y + line.extent;
}

physics::vector2d VerticalWall::getPosition() const {
	return line.position;
}

float VerticalWall::getExtent() const {
	return line.extent;
}

physics::vector2d VerticalWall::getExtents() const {
	return physics::vector2d(0.0f, line.extent);
}

const physics::VerticalLine* VerticalWall::getPhysicsObject() const {
	return &line;
}