#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/tema2/tema2_transform3D.h"
#include "lab_m1/tema1/object2D.h"
#include "lab_m1/tema1/transform2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

void Tema2::Init()
{
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("EnemyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "EnemyVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "EnemyFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("SunShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShaderSun.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShaderSun.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    playerAngleLeftRight = M_PI;
    playerAngleUpDown = 0;

    timeSinceStart = 0;
    lastShot = 0;

    generateLabirinth();
    generateBossRoom();

    // Boss Fight variables
    bossFight = false;
    bossHealth = 100;
    bossDeathTime = 0;
    bossDyingTimeLeft = 3;

    gameOver = false, gameOverPrinted = false;

    playerX = starting_pos_row * 4 - 20;
    playerY = 2;
    playerZ = -20;

    timeLeft = 100;
    health = 100;

    limbsAngle = 0;
    limbsDirection = false;

    for (int i = 0; i < labirinth_rows; i++) {
        for (int j = 0; j < labirinth_cols; j++) {
            cout << labirinth[i][j] << " ";
        }
        cout << endl;
    }

    renderCameraTarget = false;

    camera1 = new implemented::CameraTema2();
    camera1->Set(glm::vec3(playerX, playerY + 0.1, playerZ), glm::vec3(playerX, playerY + 0.1, playerZ - 3.5), glm::vec3(0, 1, 0));

    camera2 = new implemented::CameraTema2();
    camera2->Set(glm::vec3(playerX, playerY + 1.5, playerZ + 2.5), glm::vec3(playerX, playerY - 1, playerZ - 2.5), glm::vec3(0, 1, 0));
    camera2->distanceToTarget = distance(glm::vec3(playerX, playerY + 1.5, playerZ + 2.5), glm::vec3(playerX, playerY + 0.075, playerZ));

    camera1->RotateFirstPerson_OY(playerAngleLeftRight);

    camera2->RotateThirdPerson_OY(playerAngleLeftRight);

    camera = camera2;

    cout << "Player: " << playerX << " " << playerY << " " << playerZ << endl;
    cout << "Camera position: " << camera2->position.x << " " << camera2->position.y << " " << camera2->position.z << endl;
    cout << "Camera forward: " << camera2->forward.x << " " << camera2->forward.y << " " << camera2->forward.z << endl;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* healthBar = object2D::CreateSquareEnemyBody("healthBar", glm::vec3(0, 0, 0), 1, glm::vec3(0.8, 0, 0), true);
        AddMeshToList(healthBar);

        Mesh* healthBarContur = object2D::CreateSquareEnemyBody("healthBarContur", glm::vec3(0, 0, 0), 1, glm::vec3(0.8, 0, 0), false);
        AddMeshToList(healthBarContur);
    }

    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.8, 0, 0)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.8, 0, 0)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh("cube", vertices, indices);
    }

    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "sun.jpg").c_str(), GL_REPEAT);
        mapTextures["sun"] = texture;
    }

    {
        lightPosition = glm::vec3(-15, 5, -30);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }

    field_of_view = 60;
    projection_type = 0;
    projectionMatrix = glm::perspective(RADIANS(field_of_view), window->props.aspectRatio, 0.01f, 200.0f);
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    lightPosition.x = 0;
    lightPosition.y = 5 + 10 * sin((100 - timeLeft) * M_PI / 100);
    lightPosition.z = -30 * cos((100 - timeLeft) * M_PI / 100);

    if (playerInCell(glm::vec3(bossRoomRow * 4 - 20, 0, bossRoomCol * 4 - 20), 3) && bossDeathTime == 0) {
        bossFight = true;
    }

    if (playerInCell(glm::vec3(bossRoomRow * 4 - 20, 0, (bossRoomCol + 1) * 4 - 20), -0.8) && !bossFight) {
        gameOver = true;
        if (!gameOverPrinted) {
            cout << "You've won! press R to restart or Q to quit" << endl;
            gameOverPrinted = true;
        }
    }

    timeSinceStart += deltaTimeSeconds;
    
    if (!gameOver) {
        timeLeft -= deltaTimeSeconds;
    }

    if (timeLeft <= 0) {
        cout << "Time's up, you're dead" << endl;
        exit(0);
    }

    // Boss Room
    {
        // Walls
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow - 2) * 4 - 20, 1, bossRoomCol * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow + 2) * 4 - 20, 1, bossRoomCol * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow - 2) * 4 - 20, 1, (bossRoomCol + 1) * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow + 2) * 4 - 20, 1, (bossRoomCol + 1) * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow - 1) * 4 - 20, 1, (bossRoomCol + 1) * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate((bossRoomRow + 1) * 4 - 20, 1, (bossRoomCol + 1) * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 2, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));

        // Floor
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 0, bossRoomCol * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(12, 0.1, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.5, 0.5, 0.5));

        // Exit Floor
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 0, (bossRoomCol + 1) * 4 - 20);
        modelMatrix *= transform3DTema2::Scale(4, 0.1, 4);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.3, 0.3));
    }

    // BossFight boundaries
    {
        if (bossFight && bossDeathTime == 0) {
            // Exit door
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 1, (bossRoomCol + 1) * 4 - 18);
            modelMatrix *= transform3DTema2::Scale(4, 2, 0.1);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.3, 0.3));

            // Trap door
            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 1, (bossRoomCol - 1) * 4 - 22);
            modelMatrix *= transform3DTema2::Scale(4, 2, 0.1);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.3, 0.3));
        }
    }

    // Boss
    {
        if (bossDeathTime == 0) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 2, bossRoomCol * 4 - 20);
            modelMatrix *= transform3DTema2::Scale(bossHealth * 2 / 100, bossHealth * 2 / 100, bossHealth * 2 / 100);
            RenderSimpleMeshEnemy(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.1, 0.3), bossDeathTime);
        }
        else {
            if (bossDyingTimeLeft > 0) {
                bossDyingTimeLeft -= deltaTimeSeconds;

                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix *= transform3DTema2::Translate(bossRoomRow * 4 - 20, 2, bossRoomCol * 4 - 20);
                modelMatrix *= transform3DTema2::Scale(bossHealth * 2 / 100, bossHealth * 2 / 100, bossHealth * 2 / 100);
                RenderSimpleMeshEnemy(meshes["sphere"], shaders["EnemyShader"], modelMatrix, glm::vec3(0.7, 0.1, 0.3), bossDeathTime);
            }
        }
    }

    // The Sun
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPosition);
        // modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        // RenderSimpleMesh(meshes["sphere"], shaders["Simple"], modelMatrix, glm::vec3(1, 1, 1));
        RenderSun(meshes["sphere"], shaders["SunShader"], modelMatrix, glm::vec3(1, 1, 1), mapTextures["sun"]);
    }

    // Labirinth Walls
    {
        for (int i = 0; i < labirinth_rows; i++) {
            for (int j = 0; j < labirinth_cols; j++) {
                if (labirinth[i][j] == 1) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix *= transform3DTema2::Translate(i * 4 - 20, 1, j * 4 - 20);
                    modelMatrix *= transform3DTema2::Scale(4, 2, 4);
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.24, 0.9, 0.4));
                }
            }
        }

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Scale(44, 0.1, 44);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.5, 0.5, 0.5));
    }

    // Enemies
    {
        float enemySpeed = 1.5f;
        for (int i = 0; i < enemies.size(); i++) {
            EnemyLabirinth* enemy = &enemies[i];

            if (enemy->shader == shaders["LabShader"]) {
                if (playerInCell(enemy->cell_center_position, 0.5)) {
                    float angle_to_player = atan2(playerZ - enemy->enemy_position.z, playerX - enemy->enemy_position.x);

                    enemy->enemy_position.x += enemySpeed * 3 * cos(angle_to_player) * deltaTimeSeconds;
                    enemy->enemy_position.z += enemySpeed * 3 * sin(angle_to_player) * deltaTimeSeconds;

                    if (!enemy->switchMoveMode) {
                        enemy->switchMoveMode = true;
                    }

                    if (distance(glm::vec3(enemy->enemy_position.x, 0, enemy->enemy_position.z), glm::vec3(playerX, 0, playerZ)) <= 0.5 + 0.25) {
                        enemies.erase(enemies.begin() + i);
                        health -= 20;
                        cout << "health " << health << endl;

                        if (health <= 0) {
                            cout << "You're dead" << endl;
                            exit(0);
                        }

                        i--;
                        continue;
                    }
                }
                else {
                    if (enemy->switchMoveMode) {
                        enemy->switchMoveMode = false;

                        enemy->currentPosition = Down;
                        enemy->enemy_position = enemy->cell_center_position + glm::vec3(-1.5, 0, -1.5);
                    }
                    switch (enemy->currentPosition)
                    {
                    case Down:
                        enemy->enemy_position.x += enemySpeed * deltaTimeSeconds;
                        enemy->posOnLine += enemySpeed * deltaTimeSeconds;
                        break;
                    case Right:
                        enemy->enemy_position.z += enemySpeed * deltaTimeSeconds;
                        enemy->posOnLine += enemySpeed * deltaTimeSeconds;
                        break;
                    case Up:
                        enemy->enemy_position.x -= enemySpeed * deltaTimeSeconds;
                        enemy->posOnLine += enemySpeed * deltaTimeSeconds;
                        break;
                    case Left:
                        enemy->enemy_position.z -= enemySpeed * deltaTimeSeconds;
                        enemy->posOnLine += enemySpeed * deltaTimeSeconds;
                        break;
                    default:
                        break;
                    }

                    if (enemy->posOnLine >= 3) {
                        switch (enemy->currentPosition)
                        {
                        case Down:
                            enemy->currentPosition = Right;
                            enemy->enemy_position = enemy->cell_center_position + glm::vec3(1.5, 0, -1.5);
                            break;
                        case Right:
                            enemy->currentPosition = Up;
                            enemy->enemy_position = enemy->cell_center_position + glm::vec3(1.5, 0, 1.5);
                            break;
                        case Up:
                            enemy->currentPosition = Left;
                            enemy->enemy_position = enemy->cell_center_position + glm::vec3(-1.5, 0, 1.5);
                            break;
                        case Left:
                            enemy->currentPosition = Down;
                            enemy->enemy_position = enemy->cell_center_position + glm::vec3(-1.5, 0, -1.5);
                            break;
                        default:
                            break;
                        }

                        enemy->posOnLine = 0;
                    }
                }
            }
            else {
                if (enemy->explosionTimeLeft <= 0) {
                    enemies.erase(enemies.begin() + i);
                    i--;
                    continue;
                }

                enemy->explosionTimeLeft -= deltaTimeSeconds;
            }

            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(enemy->enemy_position.x, 1, enemy->enemy_position.z);
            modelMatrix *= transform3DTema2::Scale(0.5, 0.5, 0.5);
            RenderSimpleMeshEnemy(meshes["sphere"], enemy->shader, modelMatrix, glm::vec3(0.7, 0.1, 0.3), enemy->killedTime);
        }
    }
    
    // Player
    {
        // Torso
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX, playerY - 0.5, playerZ);
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.5, 0.8, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.4, 0.7, 0.7));

        // Legs
        // Right Leg
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX + 0.13 * cos(playerAngleLeftRight) + 0.2 * sin(limbsAngle * sin(playerAngleLeftRight)), playerY - 1.325, playerZ - 0.13 * sin(playerAngleLeftRight) - 0.2 * sin(limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.8, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.25, 0.3, 0.7));

        // Left Leg
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX - 0.13 * cos(playerAngleLeftRight) + 0.2 * sin(-limbsAngle * sin(playerAngleLeftRight)), playerY - 1.325, playerZ + 0.13 * sin(playerAngleLeftRight) - 0.2 * sin(-limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(-limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(-limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.8, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.25, 0.3, 0.7));

        if (camera == camera2) {
            // Head
            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(playerX, playerY + 0.075, playerZ);
            modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
            modelMatrix *= transform3DTema2::Scale(0.3, 0.3, 0.3);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.97, 0.93, 0.88));
        }
    
        // Arms
        // Right Arm
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX + 0.38 * cos(playerAngleLeftRight) + 0.12 * sin(-limbsAngle * sin(playerAngleLeftRight)), playerY - 0.34, playerZ - 0.38 * sin(playerAngleLeftRight) - 0.12 * sin(-limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(-limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(-limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.48, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.4, 0.7, 0.7));

        // Left Arm
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX - 0.38 * cos(playerAngleLeftRight) + 0.12 * sin(limbsAngle * sin(playerAngleLeftRight)), playerY - 0.34, playerZ + 0.38 * sin(playerAngleLeftRight) - 0.12 * sin(limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.48, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.4, 0.7, 0.7));

        // Forearms
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX + 0.38 * cos(playerAngleLeftRight) + 0.555 * sin(-limbsAngle * sin(playerAngleLeftRight)), playerY - 0.745 + 0.1 * abs(sin(-limbsAngle)), playerZ - 0.38 * sin(playerAngleLeftRight) - 0.555 * sin(-limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(-limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(-limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.3, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.97, 0.93, 0.88));

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(playerX - 0.38 * cos(playerAngleLeftRight) + 0.555 * sin(limbsAngle * sin(playerAngleLeftRight)), playerY - 0.745 + 0.1 * abs(sin(limbsAngle)), playerZ + 0.38 * sin(playerAngleLeftRight) - 0.555 * sin(limbsAngle * -cos(playerAngleLeftRight)));
        modelMatrix *= transform3DTema2::RotateOZ(limbsAngle * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOX(limbsAngle * -cos(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::Scale(0.23, 0.3, 0.3);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.97, 0.93, 0.88));
    }

    // Projectiles
    {
        for (int i = 0; i < projectiles.size(); i++) {
            Projectile* projectile = &projectiles[i];
            float projectileSpeed = 20.0f;

            projectile->projectile_position.x -= projectileSpeed * deltaTimeSeconds * sin(projectile->angleLeftRight) * cos(projectile->angleUpDown);
            projectile->projectile_position.z -= projectileSpeed * deltaTimeSeconds * cos(projectile->angleLeftRight) * cos(projectile->angleUpDown);
            projectile->projectile_position.y += projectileSpeed * deltaTimeSeconds * sin(projectile->angleUpDown);

            projectile->distance_travelled += sqrt((projectileSpeed * deltaTimeSeconds * sin(projectile->angleLeftRight)) * (projectileSpeed * deltaTimeSeconds * sin(projectile->angleLeftRight))
                + (projectileSpeed * deltaTimeSeconds * cos(projectile->angleLeftRight)) * (projectileSpeed * deltaTimeSeconds * cos(projectile->angleLeftRight))
                + (projectileSpeed * deltaTimeSeconds * sin(projectile->angleUpDown)) * (projectileSpeed * deltaTimeSeconds * sin(projectile->angleUpDown)));

            if (projectile->distance_travelled >= 8) {
                projectiles.erase(projectiles.begin() + i);
                i--;
                continue;
            }

            if (colisionLabirinthWalls(projectile->projectile_position.x, projectile->projectile_position.z, 0.05) && projectile->projectile_position.y >= 0 && projectile->projectile_position.y <= 2) {
                projectiles.erase(projectiles.begin() + i);
                i--;
                continue;
            }

            bool hitEnemy = false;

            for (int j = 0; j < enemies.size(); j++) {
                EnemyLabirinth* enemy = &enemies[j];

                if (colisionBulletEnemy(enemy->enemy_position, projectile->projectile_position, 0.3) && enemy->killedTime == 0) {
                    hitEnemy = true;
                    enemy->shader = shaders["EnemyShader"];
                    enemy->killedTime = timeSinceStart;

                    break;
                }
            }

            if (colisionBulletEnemy(glm::vec3(bossRoomRow * 4 - 20, 2, bossRoomCol * 4 - 20), projectile->projectile_position, 1) && bossDeathTime == 0) {
                bossHealth -= 20;
                hitEnemy = true;

                if (bossHealth == 0) {
                    bossDeathTime = timeSinceStart;
                    bossFight = false;
                    bossHealth = 20;
                }
            }

            if (hitEnemy) {
                projectiles.erase(projectiles.begin() + i);
                i--;
                continue;
            }

            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix *= transform3DTema2::Translate(projectile->projectile_position.x, projectile->projectile_position.y, projectile->projectile_position.z);
            modelMatrix *= transform3DTema2::Scale(0.1, 0.1, 0.1);
            RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0.1, 0.1));
        }
    }

    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
}


void Tema2::FrameEnd()
{
    // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);

    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(resolution.x * 3 / 4, resolution.y * 3 / 4, resolution.x / 4, resolution.y / 4);

    projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 200.0f);

    if (camera == camera2) {
        camera = camera1;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x + (0.75 - 0.75 * health / 100) * cos(playerAngleLeftRight), camera->forward.y + camera->position.y, camera->forward.z + camera->position.z - (0.75 - 0.75 * health / 100) * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5 * health / 100, 0.75, 0);
        RenderMesh(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x, camera->forward.y + camera->position.y, camera->forward.z + camera->position.z);
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5, 0.75, 0);
        RenderMesh(meshes["healthBarContur"], shaders["VertexColor"], modelMatrix);

        glViewport(0, resolution.y * 3 / 4, resolution.x / 4, resolution.y / 4);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x - (0.75 - 0.75 * timeLeft / 100) * cos(playerAngleLeftRight), camera->forward.y + camera->position.y, camera->forward.z + camera->position.z + (0.75 - 0.75 * timeLeft / 100) * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5 * timeLeft / 100, 0.75, 0);
        RenderMesh(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x, camera->forward.y + camera->position.y, camera->forward.z + camera->position.z);
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5, 0.75, 0);
        RenderMesh(meshes["healthBarContur"], shaders["VertexColor"], modelMatrix);

        camera = camera2;
    }
    else {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x + (0.75 - 0.75 * health / 100) * cos(playerAngleLeftRight), camera->forward.y + camera->position.y, camera->forward.z + camera->position.z - (0.75 - 0.75 * health / 100) * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5 * health / 100, 0.75, 0);
        RenderMesh(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x, camera->forward.y + camera->position.y, camera->forward.z + camera->position.z);
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5, 0.75, 0);
        RenderMesh(meshes["healthBarContur"], shaders["VertexColor"], modelMatrix);

        glViewport(0, resolution.y * 3 / 4, resolution.x / 4, resolution.y / 4);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x - (0.75 - 0.75 * timeLeft / 100) * cos(playerAngleLeftRight), camera->forward.y + camera->position.y, camera->forward.z + camera->position.z + (0.75 - 0.75 * timeLeft / 100) * sin(playerAngleLeftRight));
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5 * timeLeft / 100, 0.75, 0);
        RenderMesh(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3DTema2::Translate(camera->forward.x + camera->position.x, camera->forward.y + camera->position.y, camera->forward.z + camera->position.z);
        modelMatrix *= transform3DTema2::RotateOY(playerAngleLeftRight);
        modelMatrix *= transform3DTema2::RotateOX(playerAngleUpDown);
        modelMatrix *= transform3DTema2::Scale(1.5, 0.75, 0);
        RenderMesh(meshes["healthBarContur"], shaders["VertexColor"], modelMatrix);
    }

    projectionMatrix = glm::perspective(RADIANS(field_of_view), window->props.aspectRatio, 0.01f, 200.0f);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;
        
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);
        
    // Set shader uniforms for light & material properties
    // TODO(student): Set light position uniform
    int light_location = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(light_location, 1, glm::value_ptr(lightPosition));
        
    glm::vec3 eyePosition = camera->position;
    // TODO(student): Set eye position (camera position) uniform
        
    int eye_location = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(eye_location, 1, glm::value_ptr(eyePosition));
        
    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    int material_locations = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_locations, materialShininess);
        
    int material_kd_location = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd_location, materialKd);
        
    int material_ks_location = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks_location, materialKs);
        
    int object_location = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_location, 1, glm::value_ptr(color));
        
    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        
    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        
    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        
    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSun(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Set any other shader uniforms that you need
    int locTime = glGetUniformLocation(shader->program, "time");

    glUniform1f(locTime, (GLfloat)timeSinceStart);

    if (texture)
    {
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "textureUniform"), 0);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float colisionTime)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light & material properties
    // TODO(student): Set light position uniform
    int light_location = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(light_location, 1, glm::value_ptr(lightPosition));

    glm::vec3 eyePosition = camera->position;
    // TODO(student): Set eye position (camera position) uniform

    int eye_location = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(eye_location, 1, glm::value_ptr(eyePosition));

    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    int material_locations = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_locations, materialShininess);

    int material_kd_location = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd_location, materialKd);

    int material_ks_location = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks_location, materialKs);

    int object_location = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_location, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    int locationTime = glGetUniformLocation(shader->program, "Time");

    glUniform1f(locationTime, (GLfloat)timeSinceStart - colisionTime);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 2.0f;

    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
        cameraSpeed = 5.0f;
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));

        if (!colisionLabirinthWalls(playerX + dir.x * cameraSpeed * deltaTime, playerZ + dir.z * cameraSpeed * deltaTime, 0.5) &&
            !checkBossFightColision(playerX + dir.x * cameraSpeed * deltaTime, playerZ + dir.z * cameraSpeed * deltaTime, 0.5)) {
            playerX += dir.x * cameraSpeed * deltaTime;
            playerZ += dir.z * cameraSpeed * deltaTime;

            camera1->MoveForward(cameraSpeed * deltaTime);
            camera2->MoveForward(cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));

        if (!colisionLabirinthWalls(playerX - dir.x * cameraSpeed * deltaTime, playerZ - dir.z * cameraSpeed * deltaTime, 0.5) &&
            !checkBossFightColision(playerX - dir.x * cameraSpeed * deltaTime, playerZ - dir.z * cameraSpeed * deltaTime, 0.5)) {
            playerX -= dir.x * cameraSpeed * deltaTime;
            playerZ -= dir.z * cameraSpeed * deltaTime;

            camera1->MoveForward(-cameraSpeed * deltaTime);
            camera2->MoveForward(-cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        if (!colisionLabirinthWalls(playerX - cos(playerAngleLeftRight) * cameraSpeed * deltaTime, playerZ + sin(playerAngleLeftRight) * cameraSpeed * deltaTime, 0.5) && 
            !checkBossFightColision(playerX - cos(playerAngleLeftRight) * cameraSpeed * deltaTime, playerZ + sin(playerAngleLeftRight) * cameraSpeed * deltaTime, 0.5)) {
            playerX -= cameraSpeed * deltaTime * cos(playerAngleLeftRight);
            playerZ += cameraSpeed * deltaTime * sin(playerAngleLeftRight);
            camera1->TranslateRight(-cameraSpeed * deltaTime);
            camera2->TranslateRight(-cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        if (!colisionLabirinthWalls(playerX + cos(playerAngleLeftRight) * cameraSpeed * deltaTime, playerZ - sin(playerAngleLeftRight) * cameraSpeed * deltaTime, 0.5) &&
            !checkBossFightColision(playerX + cos(playerAngleLeftRight) * cameraSpeed * deltaTime, playerZ - sin(playerAngleLeftRight) * cameraSpeed * deltaTime, 0.5)) {
            playerX += cameraSpeed * deltaTime * cos(playerAngleLeftRight);
            playerZ -= cameraSpeed * deltaTime * sin(playerAngleLeftRight);
            camera1->TranslateRight(cameraSpeed * deltaTime);
            camera2->TranslateRight(cameraSpeed * deltaTime);
        }
    }

    // Shoot projectile
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && camera == camera1) {
        if (lastShot == 0 || timeSinceStart - lastShot >= 0.5) {
            projectiles.push_back(Projectile(camera1->position, playerAngleLeftRight, playerAngleUpDown));
            lastShot = timeSinceStart;
        }
    }

    if (window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_S) || window->KeyHold(GLFW_KEY_A) || window->KeyHold(GLFW_KEY_D)) {
        if (!limbsDirection) {
            limbsAngle += deltaTime;
            if (limbsAngle >= M_PI / 6) {
                limbsDirection = true;
            }
        }
        else {
            limbsAngle -= deltaTime;
            if (limbsAngle <= - M_PI / 6) {
                limbsDirection = false;
            }
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (camera == camera1)
            camera = camera2;
        else
            camera = camera1;
    }

    if (gameOver && key == GLFW_KEY_R) {
        playerAngleLeftRight = M_PI;
        playerAngleUpDown = 0;

        lastShot = 0;

        generateLabirinth();
        generateBossRoom();

        // Boss Fight variables
        bossFight = false;
        bossHealth = 100;
        bossDeathTime = 0;
        bossDyingTimeLeft = 3;

        gameOver = false, gameOverPrinted = false;

        playerX = starting_pos_row * 4 - 20;
        playerY = 2;
        playerZ = -20;

        timeSinceStart = 0;
        timeLeft = 100;
        health = 100;

        for (int i = 0; i < labirinth_rows; i++) {
            for (int j = 0; j < labirinth_cols; j++) {
                cout << labirinth[i][j] << " ";
            }
            cout << endl;
        }

        renderCameraTarget = false;

        camera1 = new implemented::CameraTema2();
        camera1->Set(glm::vec3(playerX, playerY + 0.1, playerZ), glm::vec3(playerX, playerY + 0.1, playerZ - 3.5), glm::vec3(0, 1, 0));

        camera2 = new implemented::CameraTema2();
        camera2->Set(glm::vec3(playerX, playerY + 1.5, playerZ + 2.5), glm::vec3(playerX, playerY - 1, playerZ - 2.5), glm::vec3(0, 1, 0));
        camera2->distanceToTarget = distance(glm::vec3(playerX, playerY + 1.5, playerZ + 2.5), glm::vec3(playerX, playerY + 0.075, playerZ));

        camera1->RotateFirstPerson_OY(playerAngleLeftRight);

        camera2->RotateThirdPerson_OY(playerAngleLeftRight);

        camera = camera2;

        lightPosition = glm::vec3(-15, 5, -30);
    }

    if (gameOver && key == GLFW_KEY_Q) {
        cout << "Game Over" << endl;
        exit(0);
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
    if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
        if (!window->KeyHold(GLFW_KEY_W) && !window->KeyHold(GLFW_KEY_S) && !window->KeyHold(GLFW_KEY_A) && !window->KeyHold(GLFW_KEY_D)) {
            limbsAngle = 0;
            limbsDirection = false;
        }
    }
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (deltaY < 0 && playerAngleUpDown - sensivityOX * deltaY <= 1.5) {
            camera1->RotateFirstPerson_OX(-sensivityOX * deltaY);
            camera2->RotateThirdPerson_OX(-sensivityOX * deltaY);
            playerAngleUpDown -= sensivityOX * deltaY;
        }

        if (deltaY > 0 && playerAngleUpDown - sensivityOX * deltaY >= -1) {
            camera1->RotateFirstPerson_OX(-sensivityOX * deltaY);
            camera2->RotateThirdPerson_OX(-sensivityOX * deltaY);
            playerAngleUpDown -= sensivityOX * deltaY;
        }

        camera1->RotateFirstPerson_OY(-sensivityOY * deltaX);
        camera2->RotateThirdPerson_OY(-sensivityOY * deltaX);

        playerAngleLeftRight -= sensivityOY * deltaX;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
