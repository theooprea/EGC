#pragma once

#include "components/simple_scene.h"

class Bullet {
    public:
        Bullet(glm::vec3 bullet_position, float angle) {
            this->bullet_position = bullet_position;
            this->angle = angle;
            this->distance_travelled = 0;
        }

        glm::vec3 bullet_position;
        float angle;
        float distance_travelled;
};