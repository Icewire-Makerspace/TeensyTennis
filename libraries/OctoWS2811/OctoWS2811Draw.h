#ifndef OCTOWS2811DRAW_H
#define OCTOWS2811DRAW_H

#include "OctoWS2811.h"

class OctoWS2811Draw {
public:
	OctoWS2811Draw(OctoWS2811* _leds, int _horizontalResolution, int _verticalResolution) : leds(_leds), horizontalResolution(_horizontalResolution), verticalResolution(_verticalResolution), color(0) {}
	
	void clearBuffer();
	void drawBuffer();
	
	void setColor(uint_fast8_t _color);
	
	void dot(int x, int y);
	void line(int x0, int y0, int x1, int y1);
	void rect(int x, int y, int w, int h);
	
private:
	void setPixel(int x, int y);
	
	OctoWS2811* leds;
	int horizontalResolution;
	int verticalResolution;
	
	uint_fast8_t color;
};

#endif