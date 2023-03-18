#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
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

Mesh* object2D::CreateSquareEnemy(
    const std::string& name,
    glm::vec3 center,
    float length,
    glm::vec3 color,
    glm::vec3 colorHands)
{
    glm::vec3 corner = center;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, 1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, 1.0f * length / 2, 0), color),

        VertexFormat(corner + glm::vec3(1.0f * length / 4, -3.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(3.0f * length / 4, -3.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(3.0f * length / 4, -1.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(1.0f * length / 4, -1.0f * length / 4, 0), colorHands),

        VertexFormat(corner + glm::vec3(1.0f * length / 4, 1.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(3.0f * length / 4, 1.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(3.0f * length / 4, 3.0f * length / 4, 0), colorHands),
        VertexFormat(corner + glm::vec3(1.0f * length / 4, 3.0f * length / 4, 0), colorHands),
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11
    };

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateSquareEnemyBody(
    const std::string& name,
    glm::vec3 center,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = center;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, 1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, 1.0f * length / 2, 0), color),
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateSquareBullet(
    const std::string& name,
    glm::vec3 center,
    float length,
    glm::vec3 color)
{
    glm::vec3 corner = center;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, -1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(1.0f * length / 2, 1.0f * length / 2, 0), color),
        VertexFormat(corner + glm::vec3(-1.0f * length / 2, 1.0f * length / 2, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateCircle(
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
        indices.push_back(0);
        indices.push_back(count);
        indices.push_back(1);
    }

    cirle->InitFromData(vertices, indices);
    return cirle;
}

Mesh* object2D::CreateCircleHealth(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    glm::vec3 colorCross,
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
        indices.push_back(0);
        indices.push_back(count);
        indices.push_back(1);
    }

    vertices.push_back(VertexFormat(corner + glm::vec3(-1 * radius / 6, -2 * radius / 3, 0), colorCross));
    vertices.push_back(VertexFormat(corner + glm::vec3(1 * radius / 6, -2 * radius / 3, 0), colorCross));
    vertices.push_back(VertexFormat(corner + glm::vec3(-1 * radius / 6, 2 * radius / 3, 0), colorCross));
    vertices.push_back(VertexFormat(corner + glm::vec3(1 * radius / 6, 2 * radius / 3, 0), colorCross));

    indices.push_back(count + 1);
    indices.push_back(count + 2);
    indices.push_back(count + 3);
    indices.push_back(count + 1);
    indices.push_back(count + 3);
    indices.push_back(count + 4);

    cirle->InitFromData(vertices, indices);
    return cirle;
}

