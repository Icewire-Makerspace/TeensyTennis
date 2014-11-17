#include "player.h"

void Player::setup(Paddle* _paddle, float _movementSpeed) {
	active = true;
	paddle = _paddle;
	movementSpeed = _movementSpeed;
}

const Paddle* Player::getPaddle() const {
	return paddle;
}

void Player::update(float dt) {
	paddle->updatePhysics(dt);
}

void Player::setPosition(physics::vector2d position) {
	paddle->setPosition(position);
}

void Player::moveUp() {
	paddle->setVelocity(movementSpeed);
}

void Player::moveDown() {
	paddle->setVelocity(-movementSpeed);
}

void Player::stop() {
	paddle->setVelocity(0);
}