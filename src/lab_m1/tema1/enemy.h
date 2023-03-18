#pragma once

#include "components/simple_scene.h"

class Enemy {
    public:
        Enemy(glm::vec3 enemy_position, float speed, int type) {
            this->enemy_position = enemy_position;
            this->speed = speed;
            this->type = type;
            this->lastShot = 0;
        }

        glm::vec3 enemy_position;
        float speed, lastShot;
        int type;
};
