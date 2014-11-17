#include "controller.h"

void PlayerController::moveUp() {
	u = true;
	d = false;
}

void PlayerController::moveDown() {
	u = false;
	d = true;
}

void PlayerController::stop() {
	u = false;
	d = false;
}

bool PlayerController::up() {
	return u;
}

bool PlayerController::down() {
	return d;
}