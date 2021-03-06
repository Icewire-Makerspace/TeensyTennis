#include "OctoWS2811Draw.h"

void OctoWS2811Draw::clearBuffer() {
	uint_fast8_t tmpColor = color;
	color = 0;
	for (int i = 0; i < horizontalResolution; ++i) {
		for (int j = 0; j < verticalResolution; ++j) {
			setPixel(i, j);
		}
	}
	color = tmpColor;
}

void OctoWS2811Draw::drawBuffer() {
	leds->show();
}

void OctoWS2811Draw::setColor(uint_fast8_t _color) {
	color = _color;
}

void OctoWS2811Draw::dot(int x, int y) {
	setPixel(x, y);
}

void OctoWS2811Draw::line(int x0, int y0, int x1, int y1) {
	if (y0 == y1) {
		for (int i = x0; i <= x1; ++i) {
			setPixel(i, y0);
		}
	} else if (x0 == x1) {
		for (int i = y0; i <= y1; ++i) {
			setPixel(x0, i);
		}
	} else {
		// Bresenham's line algorithm
		int dx = x1 - x0;
		int dy = y1 - y0;
		int d = 2*dy - dx;
		setPixel(x0, y0);
		int y = y0;
		for (int i = x0 + 1; i <= x1; ++i) {
			if (d > 0) {
				++y;
				setPixel(i, y);
				d += 2 * dy - 2 * dx;
			} else {
				setPixel(i, y);
				d += 2 * dy;
			}
		}
	}
}

void OctoWS2811Draw::rect(int x, int y, int w, int h) {
	int x1 = x + w;
	int y1 = y + h;
	for (int i = x; i < x1; ++i) {
		for (int j = y; j < y1; ++j) {
			setPixel(i, j);
		}
	}
}

void OctoWS2811Draw::letter(char letter, int x, int y) {
	const uint8_t* letterBmp = ascii_alph[letter-65];
	int xMax = x + ascii_width;
	int yMax = y + ascii_height;
	int bit;
	for (int j = y; j < yMax; ++j) {
		bit = 0x80;
		for (int i = x; i < xMax; ++i) {
			if (*letterBmp & bit) {
				setPixel(i, j);
			}
			bit >>= 1;
		}
		++letterBmp;
	}
	// reset pointer
	letterBmp -= ascii_height;
}

void OctoWS2811Draw::string(const char *str, int x, int y) {
	while (*str) {
		letter(*str++, x, y);
		x += 8;
	}
}

void OctoWS2811Draw::number(int num, int x, int y) {
	const uint8_t* numberBmp = ascii_num[num];
	int xMax = x + ascii_width;
	int yMax = y + ascii_height;
	int bit;
	for (int j = y; j < yMax; ++j) {
		bit = 0x80;
		for (int i = x; i < xMax; ++i) {
			if (*numberBmp & bit) {
				setPixel(i, j);
			}
			bit >>= 1;
		}
		++numberBmp;
	}
	// reset pointer
	numberBmp -= ascii_height;
}

void OctoWS2811Draw::setPixel(int x, int y) {
	// Assumes (0, 0) is your LED display's top left LED
	if (x < 0 || x >= horizontalResolution ) {
		return;
	}
	if (y < 0 || y >= verticalResolution) {
		return;
	}
	uint_fast32_t num;
	num = y * horizontalResolution;
	if (y % 2 == 0) {
		num += x;
	} else {
		num += (horizontalResolution - 1) - x;
	}
	leds->setPixel(num, color);
}