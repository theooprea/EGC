#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    // Bonus
    torsoAngle = 0;
    legAngle = 0;
    bonusZPoz = -5.5f;
}


void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Left leg lower
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-0.75f, 2 - 1.0f * cos(legAngle), bonusZPoz + sin(legAngle));
    modelMatrix *= transform3D::RotateOX(-legAngle);
    modelMatrix *= transform3D::Scale(0.5, 2, 0.75);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Left leg upper
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-0.75f, 3.0f, bonusZPoz);
    modelMatrix *= transform3D::Scale(0.5, 2, 1);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Right leg lower
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.75f, 1.0f, bonusZPoz);
    modelMatrix *= transform3D::Scale(0.5, 2, 0.75);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Right leg upper
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.75f, 3.0f, bonusZPoz);
    modelMatrix *= transform3D::Scale(0.5, 2, 1);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Torso
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, 6.0f, bonusZPoz);
    modelMatrix *= transform3D::Scale(2, 4, 2);
    modelMatrix *= transform3D::RotateOY(torsoAngle);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Left hand
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-1.25f * cos(torsoAngle), 6.5f, bonusZPoz + 1.25f * sin(torsoAngle));
    modelMatrix *= transform3D::Scale(0.5, 3, 0.5);
    modelMatrix *= transform3D::RotateOY(torsoAngle);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Right hand
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.25f * cos(torsoAngle), 6.5f, bonusZPoz - 1.25f * sin(torsoAngle));
    modelMatrix *= transform3D::Scale(0.5, 3, 0.5);
    modelMatrix *= transform3D::RotateOY(torsoAngle);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Head
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 8.5f, bonusZPoz);
    modelMatrix *= transform3D::Scale(1, 1, 1);
    modelMatrix *= transform3D::RotateOY(torsoAngle);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}


void Lab4::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // TODO(student): Add transformation logic
    if (window->KeyHold(GLFW_KEY_W) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateZ -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateX -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateX += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_R) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateY += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_F) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        translateY -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_1)) {
        scaleX += 1 * deltaTime;
        scaleY += 1 * deltaTime;
        scaleZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        scaleX -= 1 * deltaTime;
        scaleY -= 1 * deltaTime;
        scaleZ -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_3)) {
        angularStepOX += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4)) {
        angularStepOX -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_5)) {
        angularStepOY += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6)) {
        angularStepOY -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_7)) {
        angularStepOZ += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_8)) {
        angularStepOZ -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_U)) {
        torsoAngle += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Y)) {
        torsoAngle -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_I)) {
        legAngle += 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_O)) {
        legAngle -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_P)) {
        bonusZPoz -= 2 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_L)) {
        bonusZPoz += 2 * deltaTime;
    }
}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
