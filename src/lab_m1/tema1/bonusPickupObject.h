#pragma once

#include "components/simple_scene.h"

class bonusPickupObject {
public:
    bonusPickupObject(glm::vec3 obj_position, int type) {
        this->obj_position = obj_position;
        this->type = type;
    }

    glm::vec3 obj_position;
    int type;
};