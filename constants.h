#ifndef CONSTANTS_H
#define CONSTANTS_H

const int PHYSICS_TO_PIXEL_RATIO = 20;
const int SCREEN_HEIGHT = 480;

inline float physicsToScreen(float p) {
	return p * PHYSICS_TO_PIXEL_RATIO;
}

inline float physicsToScreenX(float x) {
	return physicsToScreen(x);
}

inline float physicsToScreenY(float y) {
	return SCREEN_HEIGHT - (physicsToScreen(y));
}

#endif