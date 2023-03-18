#pragma once

#include "components/simple_scene.h"

class Projectile {
public:
    Projectile(glm::vec3 projectile_position, float angleLeftRight, float angleUpDown) {
        this->projectile_position = projectile_position;
        this->angleLeftRight = angleLeftRight;
        this->angleUpDown = angleUpDown;
        this->distance_travelled = 0;
    }

    glm::vec3 projectile_position;
    float angleLeftRight, angleUpDown;
    float distance_travelled;
};