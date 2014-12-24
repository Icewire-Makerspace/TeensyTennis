#ifndef OBJECTS2D_H
#define OBJECTS2D_H

#include "math2d.h"

namespace physics {

	struct Line {
		vector2d position;
		float extent;
	};

	struct HorizontalLine : Line {};

	struct VerticalLine : Line {};

	struct Box {
		vector2d position;
		vector2d extents;
	};

	struct MovingBox : Box {
		vector2d velocity;
	};

}

#endif