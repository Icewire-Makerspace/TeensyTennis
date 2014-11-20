#include "paddle.h"

void Paddle::setup(physics::vector2d p, float length, float height) {
	box.extents.x = length * 0.5f;
	box.extents.y = height * 0.5f;
	box.position.x = p.x + box.extents.x;
	box.position.y = p.y + box.extents.y;
	box.velocity = physics::VECTOR2D_ZERO;
}

void Paddle::setPosition(physics::vector2d position) {
	box.position = position;
}

physics::vector2d Paddle::getPosition() const {
	return box.position;
}

physics::vector2d Paddle::getExtents() const {
	return box.extents;
}

void Paddle::setVelocity(float velocity) {
	box.velocity.y = velocity;
}

void Paddle::updatePhysics(float dt) {
	box.position += box.velocity * dt;
}

const physics::MovingBox* Paddle::getPhysicsObject() const {
	return &box;
}