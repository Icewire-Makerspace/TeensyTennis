#ifndef BALL_H
#define BALL_H

#include "physics\objects2d.h"

class Ball {
public:
	void setup(physics::vector2d p, float diameter, physics::vector2d velocity);
	void setPosition(physics::vector2d position);
	physics::vector2d getPosition() const;
	float getRadius() const;
	void reverseVelocityX();
	void reverseVelocityY();
	void setVelocityX(float speed);
	float getVelocityX();
	void setVelocityY(float speed);
	float getVelocityY();
	void increaseVelocity(float mult);
	void updatePhysics(float dt);
	const physics::MovingBox* getPhysicsObject() const;

private:
	physics::MovingBox box;
};

#endif