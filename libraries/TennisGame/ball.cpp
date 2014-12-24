#include "ball.h"

void Ball::setup(physics::vector2d p, float diameter, physics::vector2d velocity) {
	box.extents.x = diameter * 0.5f;
	box.extents.y = box.extents.x;
	box.position.x = p.x + box.extents.x;
	box.position.y = p.y + box.extents.y;
	box.velocity = velocity;
}

void Ball::setPosition(physics::vector2d position) {
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

float Ball::getVelocityX() {
	return box.velocity.x;
}

void Ball::setVelocityY(float speed) {
	box.velocity.y = speed;
}

float Ball::getVelocityY() {
	return box.velocity.y;
}

void Ball::increaseVelocity(float mult) {
	box.velocity *= mult;
}

void Ball::updatePhysics(float dt) {
	box.position += box.velocity * dt;
}

const physics::MovingBox* Ball::getPhysicsObject() const {
	return &box;
}