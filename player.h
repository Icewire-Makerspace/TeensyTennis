#ifndef PLAYER_H
#define PLAYER_H

#include "paddle.h"

class Player {
public:
	void setup(Paddle* _paddle, float _movementSpeed);
	const Paddle* getPaddle() const;
	void update(float dt);

	// Position
	void setPosition(physics::vector2d position);

	// Controls
	void moveUp();
	void moveDown();
	void stop();

	bool active;

private:
	Paddle *paddle;
	float movementSpeed;
};

#endif