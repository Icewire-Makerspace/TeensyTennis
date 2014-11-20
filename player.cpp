#include "player.h"

void Player::setup(Paddle* _paddle, float _maxMovementSpeed) {
	paddle = _paddle;
	maxMovementSpeed = _maxMovementSpeed;
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

void Player::changeSpeedTo(float speed) {
	if (speed > maxMovementSpeed) {
		speed = maxMovementSpeed;
	} else if (speed < -maxMovementSpeed) {
		speed = -maxMovementSpeed;
	}
	
	paddle->setVelocity(speed);
}

void Player::stop() {
	paddle->setVelocity(0);
}