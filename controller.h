#ifndef CONTROLLER_H
#define CONTROLLER_H

class PlayerController {
public:
	PlayerController() : speed(0) {}
	void setSpeed(float _speed);
	float getSpeed();

private:
	float speed;
};

#endif