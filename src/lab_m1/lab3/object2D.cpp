#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquareLab(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateCircleLab(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = center;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(radius * cos(0), radius * sin(0), 0), color),
    };

    std::vector<unsigned int> indices =
    {

    };

    const float PI = 3.1415926f;
    float increment = 2.0f * PI / 100;
    int count = 1;

    for (float angle = increment; angle < 2.0f * PI; angle += increment) {
        vertices.push_back(VertexFormat(corner + glm::vec3(radius * cos(angle), radius * sin(angle), 0), color));
        indices.push_back(0);
        indices.push_back(count);
        indices.push_back(count + 1);
        count++;
    }

    Mesh* cirle = new Mesh(name);

    if (!fill) {
        cirle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(count);
        indices.push_back(1);
    }

    cirle->InitFromData(vertices, indices);
    return cirle;
}
