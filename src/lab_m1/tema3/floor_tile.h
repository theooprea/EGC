#pragma once

#include "components/simple_scene.h"

class FloorTile {
public:
	FloorTile(int x, int y, glm::vec3 colour) {
		this->colour = colour;
		this->x = x;
		this->y = y;
	}
	glm::vec3 colour;
	int x, y;
};