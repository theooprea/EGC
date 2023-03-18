#pragma once

#include "components/simple_scene.h"

enum posInCell {Up, Down, Left, Right};

class EnemyLabirinth {
public:
	EnemyLabirinth(glm::vec3 cell_center_position, glm::vec3 enemy_position, posInCell initialPos, Shader *shader) {
		this->shader = shader;
		this->enemy_position = enemy_position;
		this->cell_center_position = cell_center_position;
		posOnLine = 0;
		killedTime = 0;
		explosionTimeLeft = 3;
		currentPosition = initialPos;
		switchMoveMode = false;
	}
	Shader* shader;
	glm::vec3 enemy_position;
	glm::vec3 cell_center_position;
	float posOnLine, killedTime, explosionTimeLeft;
	posInCell currentPosition;
	bool switchMoveMode;
};