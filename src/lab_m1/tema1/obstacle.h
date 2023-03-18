#pragma once

#include "components/simple_scene.h"

class Obstacle {
    public:
        Obstacle(glm::vec3 lower_left, glm::vec3 lower_right, glm::vec3 upper_left, glm::vec3 upper_right, glm::vec3 scale) {
            this->lower_left = lower_left;
            this->lower_right = lower_right;
            this->upper_left = upper_left;
            this->upper_right = upper_right;
            this->scale = scale;
        }

        glm::vec3 lower_left, lower_right, upper_left, upper_right, scale;
};
