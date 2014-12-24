#ifndef PLAYER_H
#define PLAYER_H

#include "paddle.h"

class Player {
public:
	Player() : active(true) {}
	void setup(Paddle* _paddle, float _maxMovementSpeed);
	const Paddle* getPaddle() const;
	void update(float dt);

	// Position
	void setPosition(physics::vector2d position);

	// Controls
	void changeVerticalSpeedTo(float speed);
	void changeHorizontalSpeedTo(float speed);
	void stop();

	bool active;

private:
	Paddle *paddle;
	float maxMovementSpeed;
};

#endif