#ifndef PADDLE_H
#define PADDLE_H

#include "physics\objects2d.h"

class Paddle {
public:
	void setup(physics::vector2d p, float length, float height);
	void setPosition(physics::vector2d position);
	physics::vector2d getPosition() const;
	physics::vector2d getExtents() const;
	void setVelocity(physics::vector2d velocity);
	void setVelocityX(float speed);
	void setVelocityY(float speed);
	physics::vector2d getVelocity() const;
	void updatePhysics(float dt);
	const physics::MovingBox* getPhysicsObject() const;

private:
	physics::MovingBox box;
};

#endif