#include "ball.h"

void Ball::setup(physics::vector2d p, float diameter, physics::vector2d velocity) {
	box.extents.x = diameter * 0.5f;
	box.extents.y = box.extents.x;
	box.position.x = p.x + box.extents.x;
	box.position.y = p.y + box.extents.y;
	box.velocity = velocity;
}

void Ball::setPosition(physics::vector2d position) {
	// tmp
	if (position.y < 1 || position.y > 23) {
		box.velocity.x = 0;
	}

	box.position = position;
}

physics::vector2d Ball::getPosition() const {
	return box.position;
}

float Ball::getRadius() const {
	return box.extents.x;
}

void Ball::reverseVelocityX() {
	box.velocity.x *= -1.0f;
}

void Ball::reverseVelocityY() {
	box.velocity.y *= -1.0f;
}

void Ball::setVelocityX(float speed) {
	box.velocity.x = speed;
}

void Ball::setVelocityY(float speed) {
	box.velocity.y = speed;
}

void Ball::increaseVelocity(float multiplier) {
	box.velocity *= multiplier;
}

void Ball::updatePhysics(float dt) {
	// tmp
	physics::vector2d position = box.position + box.velocity * dt;
	if (position.y < 1 || position.y > 23) {
		box.velocity.x = 0;
	}

	box.position += box.velocity * dt;
}

const physics::MovingBox* Ball::getPhysicsObject() const {
	return &box;
}