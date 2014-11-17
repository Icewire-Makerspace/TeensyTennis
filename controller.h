#ifndef CONTROLLER_H
#define CONTROLLER_H

class PlayerController {
public:
	PlayerController() : u(false), d(false) {}

	void moveUp();
	void moveDown();
	void stop();

	bool up();
	bool down();

private:
	bool u;
	bool d;
};

#endif