#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 5;       // logic x
    logicSpace.y = 5;       // logic y
    logicSpace.width = 10;   // logic width
    logicSpace.height = 10;  // logic height

    healthBarSpace.x = 5;
    healthBarSpace.y = 5;
    healthBarSpace.width = 10;
    healthBarSpace.height = 10;

    playerX = 10;
    playerY = 10;

    lookingAngle = 0;
    playerRadius = 0.5;

    timeSinceStart = 0;
    lastShot = 0;
    lastEnemySpanned = 0;

    eyesRadius = 0.15f;
    outlineRadius = 0.05;

    health = 100;
    score = 0;

    bonusRadius = playerRadius / 2;
    bulletSize = playerRadius / 4;

    shutgunTime = 0;
    machinegunTime = 0;

    bonusPickUpTime = 0;

    glm::vec3 corner = glm::vec3(0, 0, 0);
    length = 1;

    Mesh* healthBar = object2D::CreateSquare("healthBar", corner, length, glm::vec3(0.8, 0, 0), true);
    AddMeshToList(healthBar);

    Mesh* healthBarContur = object2D::CreateSquare("healthBarContur", corner, length, glm::vec3(0.8, 0, 0), false);
    AddMeshToList(healthBarContur);

    Mesh* obstacle = object2D::CreateSquare("obstacle", corner, length, glm::vec3(0.25, 0.9, 0.4), true);
    AddMeshToList(obstacle);

    Mesh* obstacleOutline = object2D::CreateSquare("obstacleOutline", corner, length, glm::vec3(0.13, 0.13, 0.13), true);
    AddMeshToList(obstacleOutline);

    Mesh* background = object2D::CreateSquare("background", corner, 20, glm::vec3(0.5, 0.5, 0.5), true);
    AddMeshToList(background);

    Mesh* circleBody = object2D::CreateCircle("circleBody", corner, playerRadius, glm::vec3(0.95, 0.8, 0.4), true);
    AddMeshToList(circleBody);

    Mesh* circleBodyOutline = object2D::CreateCircle("circleBodyOutline", corner, playerRadius + outlineRadius, glm::vec3(0.13, 0.13, 0.13), true);
    AddMeshToList(circleBodyOutline);

    Mesh* circleEyes = object2D::CreateCircle("circleEyes", corner, eyesRadius, glm::vec3(0, 0, 1), true);
    AddMeshToList(circleEyes);

    Mesh* circleEyesOutline = object2D::CreateCircle("circleEyesOutline", corner, eyesRadius + outlineRadius * 3 / 4, glm::vec3(0.13, 0.13, 0.13), true);
    AddMeshToList(circleEyesOutline);

    Mesh* squareEnemy = object2D::CreateSquareEnemy("squareEnemy", corner, 2 * playerRadius, glm::vec3(0.7333, 0.1, 0.25), glm::vec3(0.7333, 0.1, 0.25));
    AddMeshToList(squareEnemy);

    {
        Mesh* squareEnemyBody = object2D::CreateSquareEnemyBody("squareEnemyBody", corner, length, glm::vec3(0.7333, 0.1, 0.25), true);
        AddMeshToList(squareEnemyBody);

        Mesh* squareEnemyBodyShooter = object2D::CreateSquareEnemyBody("squareEnemyBodyShooter", corner, length, glm::vec3(0.13, 0.13, 0.13), true);
        AddMeshToList(squareEnemyBodyShooter);

        Mesh* squareEnemyBodyOutline = object2D::CreateSquareEnemyBody("squareEnemyBodyOutline", corner, length + outlineRadius, glm::vec3(0.13, 0.13, 0.13), true);
        AddMeshToList(squareEnemyBodyOutline);

        Mesh* squareEnemyBodyHand = object2D::CreateSquareEnemyBody("squareEnemyBodyHand", corner, length / 2, glm::vec3(0.7333, 0.1, 0.25), true);
        AddMeshToList(squareEnemyBodyHand);

        Mesh* squareEnemyBodyHandOutline = object2D::CreateSquareEnemyBody("squareEnemyBodyHandOutline", corner, length / 2 + outlineRadius, glm::vec3(0.13, 0.13, 0.13), true);
        AddMeshToList(squareEnemyBodyHandOutline);
    }

    {
        Mesh* healthBonus = object2D::CreateCircle("healthBonus", corner, bonusRadius, glm::vec3(1, 0, 0), true);
        AddMeshToList(healthBonus);

        Mesh* healthBonusCross = object2D::CreateSquareEnemyBody("healthBonusCross", corner, 4 * bonusRadius / 3, glm::vec3(0, 1, 0), true);
        AddMeshToList(healthBonusCross);
    }

    Mesh* squareBullet = object2D::CreateSquareBullet("squareBullet", corner, bulletSize, glm::vec3(0.13, 0.13, 0.13));
    AddMeshToList(squareBullet);

    Mesh* squareBulletEnemy = object2D::CreateSquareBullet("squareBulletEnemy", corner, bulletSize, glm::vec3(0.7333, 0.1, 0.25));
    AddMeshToList(squareBulletEnemy);

    bonuses.push_back(bonusPickupObject(glm::vec3(14, 9, 0), 1));
    bonuses.push_back(bonusPickupObject(glm::vec3(14, 8, 0), 2));
    bonuses.push_back(bonusPickupObject(glm::vec3(14, 7, 0), 3));

    // obstacle left low 1 & 2
    obstacles.push_back(Obstacle(glm::vec3(4, 2, 0), glm::vec3(6, 2, 0), glm::vec3(4, 8, 0), glm::vec3(6, 8, 0), glm::vec3(2, 6, 0)));
    obstacles.push_back(Obstacle(glm::vec3(6, 2, 0), glm::vec3(9, 2, 0), glm::vec3(6, 3, 0), glm::vec3(9, 3, 0), glm::vec3(3, 1, 0)));

    // obstacle right low 3
    obstacles.push_back(Obstacle(glm::vec3(15, 2, 0), glm::vec3(18, 2, 0), glm::vec3(15, 5, 0), glm::vec3(18, 5, 0), glm::vec3(3, 3, 0)));

    // obstacle left upper 6
    obstacles.push_back(Obstacle(glm::vec3(3, 15, 0), glm::vec3(5, 15, 0), glm::vec3(3, 17, 0), glm::vec3(5, 17, 0), glm::vec3(2, 2, 0)));

    // obstacle middle upper 5
    obstacles.push_back(Obstacle(glm::vec3(7, 14, 0), glm::vec3(12, 14, 0), glm::vec3(7, 18, 0), glm::vec3(12, 18, 0), glm::vec3(5, 4, 0)));

    // obstacle right upper 4
    obstacles.push_back(Obstacle(glm::vec3(15, 11, 0), glm::vec3(18, 11, 0), glm::vec3(15, 14, 0), glm::vec3(18, 14, 0), glm::vec3(3, 3, 0)));

    // obstacle 7
    obstacles.push_back(Obstacle(glm::vec3(22, 3, 0), glm::vec3(24, 3, 0), glm::vec3(22, 8, 0), glm::vec3(24, 8, 0), glm::vec3(2, 5, 0)));

    // obstacle 8
    obstacles.push_back(Obstacle(glm::vec3(30, 2, 0), glm::vec3(35, 2, 0), glm::vec3(30, 3, 0), glm::vec3(35, 3, 0), glm::vec3(5, 1, 0)));

    // obstacle 9
    obstacles.push_back(Obstacle(glm::vec3(35, 2, 0), glm::vec3(37, 2, 0), glm::vec3(35, 6, 0), glm::vec3(37, 6, 0), glm::vec3(2, 4, 0)));

    // obstacle 10
    obstacles.push_back(Obstacle(glm::vec3(28, 9, 0), glm::vec3(31, 9, 0), glm::vec3(28, 12, 0), glm::vec3(31, 12, 0), glm::vec3(3, 3, 0)));

    // obstacle 11
    obstacles.push_back(Obstacle(glm::vec3(36, 10, 0), glm::vec3(38, 10, 0), glm::vec3(36, 16, 0), glm::vec3(38, 16, 0), glm::vec3(2, 6, 0)));

    // obstacle 12
    obstacles.push_back(Obstacle(glm::vec3(32, 14, 0), glm::vec3(34, 14, 0), glm::vec3(32, 18, 0), glm::vec3(34, 18, 0), glm::vec3(2, 4, 0)));

    // obstacle 13
    obstacles.push_back(Obstacle(glm::vec3(26, 16, 0), glm::vec3(32, 16, 0), glm::vec3(26, 18, 0), glm::vec3(32, 18, 0), glm::vec3(6, 2, 0)));

    // obstacle 14
    obstacles.push_back(Obstacle(glm::vec3(24, 14, 0), glm::vec3(26, 14, 0), glm::vec3(24, 18, 0), glm::vec3(26, 18, 0), glm::vec3(2, 4, 0)));
}


// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    timeSinceStart += deltaTimeSeconds;
    if (shutgunTime > 0) {
        shutgunTime -= deltaTimeSeconds;
    }
    if (machinegunTime > 0) {
        machinegunTime -= deltaTimeSeconds;
    }

    glm::ivec2 resolution = window->GetResolution();

    if (timeSinceStart - lastEnemySpanned >= 5 || lastEnemySpanned == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> wallRand(1, 5);
        std::uniform_int_distribution<> coordRand(1, 19);
        std::uniform_int_distribution<> speedRand(300, 600);
        std::uniform_int_distribution<> enemyTypeRand(1, 10);

        int wall = wallRand(gen);
        int coord = coordRand(gen);
        float speed = (float)speedRand(gen)/ 100;
        int enemyType = enemyTypeRand(gen);

        if (wall == 1) {
            if (enemyType <= 7) {
                enemies.push_back(Enemy(glm::vec3(coord, 1, 0), speed, 1));
            }
            else {
                enemies.push_back(Enemy(glm::vec3(coord, 1, 0), speed / 2 + 0.5, 2));
            }
        }
        else if (wall == 2) {
            if (enemyType <= 7) {
                enemies.push_back(Enemy(glm::vec3(39, coord, 0), speed, 1));
            }
            else {
                enemies.push_back(Enemy(glm::vec3(39, coord, 0), speed / 2 + 0.5, 2));
            }
        }
        else if (wall == 3) {
            if (enemyType <= 7) {
                enemies.push_back(Enemy(glm::vec3(coord, 19, 0), speed, 1));
            }
            else {
                enemies.push_back(Enemy(glm::vec3(coord, 19, 0), speed / 2 + 0.5, 2));
            }
        }
        else if (wall == 4) {
            if (enemyType <= 7) {
                enemies.push_back(Enemy(glm::vec3(1, coord, 0), speed, 1));
            }
            else {
                enemies.push_back(Enemy(glm::vec3(1, coord, 0), speed / 2 + 0.5, 2));
            }
        }
        else {
            if (enemyType <= 7) {
                enemies.push_back(Enemy(glm::vec3(20, coord, 0), speed, 1));
            }
            else {
                enemies.push_back(Enemy(glm::vec3(20, coord, 0), speed / 2 + 0.5, 2));
            }
        }

        lastEnemySpanned = timeSinceStart;
    }
    

    if (timeSinceStart - bonusPickUpTime >= 20 || bonusPickUpTime == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> zoneRand(1, 5);
        std::uniform_int_distribution<> typeRand(1, 3);

        int zone = zoneRand(gen);
        int type = typeRand(gen);

        if (zone == 1) {
            std::uniform_int_distribution<> xRand(7, 14);
            std::uniform_int_distribution<> yRand(4, 13);

            int x = xRand(gen);
            int y = yRand(gen);

            bonuses.push_back(bonusPickupObject(glm::vec3(x, y, 0), type));
        }
        else if (zone == 2) {
            std::uniform_int_distribution<> xRand(15, 21);
            std::uniform_int_distribution<> yRand(6, 10);

            int x = xRand(gen);
            int y = yRand(gen);

            bonuses.push_back(bonusPickupObject(glm::vec3(x, y, 0), type));
        }
        else if (zone == 3) {
            std::uniform_int_distribution<> xRand(19, 27);
            std::uniform_int_distribution<> yRand(9, 13);

            int x = xRand(gen);
            int y = yRand(gen);

            bonuses.push_back(bonusPickupObject(glm::vec3(x, y, 0), type));
        }
        else if (zone == 4) {
            std::uniform_int_distribution<> xRand(25, 34);
            std::uniform_int_distribution<> yRand(4, 8);

            int x = xRand(gen);
            int y = yRand(gen);

            bonuses.push_back(bonusPickupObject(glm::vec3(x, y, 0), type));
        }
        else {
            std::uniform_int_distribution<> xRand(14, 23);
            std::uniform_int_distribution<> yRand(15, 19);

            int x = xRand(gen);
            int y = yRand(gen);

            bonuses.push_back(bonusPickupObject(glm::vec3(x, y, 0), type));
        }

        bonusPickUpTime = timeSinceStart;
    }

    // Sets the screen area where to draw
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute the 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    DrawScene(visMatrix, deltaTimeSeconds);
}


void Tema1::FrameEnd()
{
}

void Tema1::DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds)
{
    // HealthBar
    {
        modelMatrix = visMatrix * transform2D::Translate(playerX + 5.5f + (3 - (health * 3 / 100)), playerY + 3.5f);
        modelMatrix *= transform2D::Scale(health * 3 / 100, 1);
        RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(playerX + 5.5f, playerY + 3.5f);
        modelMatrix *= transform2D::Scale(3, 1);
        RenderMesh2D(meshes["healthBarContur"], shaders["VertexColor"], modelMatrix);
    }
    
    // Obstacles
    {
        for (Obstacle& obstacle : obstacles) {
            modelMatrix = visMatrix * transform2D::Translate(obstacle.lower_left.x, obstacle.lower_left.y);
            modelMatrix *= transform2D::Scale(obstacle.scale.x, obstacle.scale.y);
            RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Enemies
    {
        for (int i = 0; i < enemies.size(); i++) {
            Enemy* enemy = &enemies[i];
            float angle_to_player = atan2(playerY - enemy->enemy_position.y, playerX - enemy->enemy_position.x);

            if (enemy->enemy_position.x + enemy->speed * cos(angle_to_player) * deltaTimeSeconds >= 3 * playerRadius / 2 && enemy->enemy_position.x + enemy->speed * cos(angle_to_player) * deltaTimeSeconds <= 40 - (3 * playerRadius / 2)) {
                enemy->enemy_position.x += enemy->speed * cos(angle_to_player) * deltaTimeSeconds;
            }

            if (enemy->enemy_position.y + enemy->speed * sin(angle_to_player) * deltaTimeSeconds >= 3 * playerRadius / 2 && enemy->enemy_position.y + enemy->speed * sin(angle_to_player) * deltaTimeSeconds <= 40 - (3 * playerRadius / 2)) {
                enemy->enemy_position.y += enemy->speed * sin(angle_to_player) * deltaTimeSeconds;
            }

            //if (!checkPlayerObstacleColision(enemy->enemy_position.x + 2 * cos(angle_to_player) * deltaTimeSeconds, enemy->enemy_position.y + 2 * sin(angle_to_player) * deltaTimeSeconds)) {
            //    enemy->enemy_position.x += 2 * cos(angle_to_player) * deltaTimeSeconds;
            //    enemy->enemy_position.y += 2 * sin(angle_to_player) * deltaTimeSeconds;
            //}
            //else {
            //    if (!checkPlayerObstacleColision(enemy->enemy_position.x + 2 * cos(angle_to_player) * deltaTimeSeconds, enemy->enemy_position.y)) {
            //        if (cos(angle_to_player) < 0) {
            //            enemy->enemy_position.x -= 2 * deltaTimeSeconds;
            //        }
            //        else {
            //            enemy->enemy_position.x += 2 * deltaTimeSeconds;
            //        }
            //    }
            //    else if (!checkPlayerObstacleColision(enemy->enemy_position.x, enemy->enemy_position.y + 2 * sin(angle_to_player) * deltaTimeSeconds)) {
            //        if (sin(angle_to_player) < 0) {
            //            enemy->enemy_position.y -= 2 * deltaTimeSeconds;
            //        }
            //        else {
            //            enemy->enemy_position.y += 2 * deltaTimeSeconds;
            //        }
            //    }
            //    /*else {
            //        cout << "cazul asta" << endl;
            //        enemy->enemy_position.x -= 50 * cos(angle_to_player) * deltaTimeSeconds;
            //        enemy->enemy_position.y -= 50 * sin(angle_to_player) * deltaTimeSeconds;
            //    }*/
            //}

            if (enemy->type == 2 && (timeSinceStart - enemy->lastShot >= 1 || enemy->lastShot == 0) && !checkBulletObstacle(enemy->enemy_position.x + playerRadius * cos(angle_to_player), enemy->enemy_position.y + playerRadius * sin(angle_to_player))) {
                enemyBullets.push_back(Bullet(glm::vec3(enemy->enemy_position.x + playerRadius * cos(angle_to_player), enemy->enemy_position.y + playerRadius * sin(angle_to_player), 0), angle_to_player));
                enemy->lastShot = timeSinceStart;
            }

            if (checkPlayerEnemyColision(playerX, playerY, *enemy, angle_to_player)) {
                enemies.erase(enemies.begin() + i);
                health -= 20;

                if (health <= 0) {
                    cout << "You're dead" << endl;
                    exit(0);
                }

                i--;
                continue;
            }

            if (enemy->type == 1) {
                // Left Hand
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player + M_PI / 4), enemy->enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player + M_PI / 4));
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBodyHand"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyHandOutline"], shaders["VertexColor"], modelMatrix);

                // Right Hand
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player - M_PI / 4), enemy->enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player - M_PI / 4));
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBodyHand"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyHandOutline"], shaders["VertexColor"], modelMatrix);

                // Body
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x, enemy->enemy_position.y);
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBody"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyOutline"], shaders["VertexColor"], modelMatrix);
            }
            else {
                // Left Hand
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player + M_PI / 4), enemy->enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player + M_PI / 4));
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBodyHand"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyHandOutline"], shaders["VertexColor"], modelMatrix);

                // Right Hand
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player - M_PI / 4), enemy->enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player - M_PI / 4));
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBodyHand"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyHandOutline"], shaders["VertexColor"], modelMatrix);

                // Body
                modelMatrix = visMatrix * transform2D::Translate(enemy->enemy_position.x, enemy->enemy_position.y);
                modelMatrix *= transform2D::Rotate(angle_to_player);
                RenderMesh2D(meshes["squareEnemyBodyShooter"], shaders["VertexColor"], modelMatrix);
                RenderMesh2D(meshes["squareEnemyBodyOutline"], shaders["VertexColor"], modelMatrix);
            }
            
        }
    }

    // Bullets
    {
        for (int i = 0; i < bullets.size(); i++) {
            Bullet* bullet = &bullets[i];

            bullet->bullet_position.x += 6 * cos(bullet->angle) * deltaTimeSeconds;
            bullet->bullet_position.y += 6 * sin(bullet->angle) * deltaTimeSeconds;
            bullet->distance_travelled += sqrt((6 * cos(bullet->angle) * deltaTimeSeconds) * (6 * cos(bullet->angle) * deltaTimeSeconds) + (6 * sin(bullet->angle) * deltaTimeSeconds) * (6 * sin(bullet->angle) * deltaTimeSeconds));

            if (bullet->distance_travelled >= 8) {
                bullets.erase(bullets.begin() + i);
                i--;
                continue;
            }

            if (checkBulletObstacle(bullet->bullet_position.x, bullet->bullet_position.y)) {
                bullets.erase(bullets.begin() + i);
                i--;
                continue;
            }

            if (bullet->bullet_position.x <= 0 || bullet->bullet_position.x >= 40 || bullet->bullet_position.y <= 0 || bullet->bullet_position.y >= 20) {
                bullets.erase(bullets.begin() + i);
                i--;
                continue;
            }

            bool hitEnemy = false;

            for (int j = 0; j < enemies.size(); j++) {
                float angle_to_player = atan2(playerY - enemies[j].enemy_position.y, playerX - enemies[j].enemy_position.x);
                if (checkBulletEnemy(enemies[j], *bullet, angle_to_player)) {
                    hitEnemy = true;
                    score += (int)(enemies[j].speed * 100);
                    enemies.erase(enemies.begin() + j);

                    cout << "Score: " << score << endl;

                    break;
                }
            }

            if (hitEnemy) {
                bullets.erase(bullets.begin() + i);
                i--;
                continue;
            }

            modelMatrix = visMatrix * transform2D::Translate(bullet->bullet_position.x, bullet->bullet_position.y);
            modelMatrix *= transform2D::Rotate(bullet->angle);

            RenderMesh2D(meshes["squareBullet"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Enemy Bullets
    {
        for (int i = 0; i < enemyBullets.size(); i++) {
            Bullet* enemyBullet = &enemyBullets[i];

            enemyBullet->bullet_position.x += 5 * cos(enemyBullet->angle) * deltaTimeSeconds;
            enemyBullet->bullet_position.y += 5 * sin(enemyBullet->angle) * deltaTimeSeconds;
            enemyBullet->distance_travelled += sqrt((5 * cos(enemyBullet->angle) * deltaTimeSeconds) * (5 * cos(enemyBullet->angle) * deltaTimeSeconds) + (5 * sin(enemyBullet->angle) * deltaTimeSeconds) * (5 * sin(enemyBullet->angle) * deltaTimeSeconds));

            if (enemyBullet->distance_travelled >= 8) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
                continue;
            }

            if (checkBulletObstacle(enemyBullet->bullet_position.x, enemyBullet->bullet_position.y)) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
                continue;
            }

            if (enemyBullet->bullet_position.x <= 0 || enemyBullet->bullet_position.x >= 40 || enemyBullet->bullet_position.y <= 0 || enemyBullet->bullet_position.y >= 20) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
                continue;
            }

            if (sqrt((playerX - enemyBullet->bullet_position.x) * (playerX - enemyBullet->bullet_position.x) + (playerY - enemyBullet->bullet_position.y) * (playerY - enemyBullet->bullet_position.y)) <= playerRadius + eyesRadius) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;

                health -= 10;

                if (health <= 0) {
                    cout << "You're dead" << endl;
                    exit(0);
                }

                continue;
            }

            modelMatrix = visMatrix * transform2D::Translate(enemyBullet->bullet_position.x, enemyBullet->bullet_position.y);
            modelMatrix *= transform2D::Rotate(enemyBullet->angle);

            RenderMesh2D(meshes["squareBulletEnemy"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Player
    {
        modelMatrix = visMatrix * transform2D::Translate(playerX + playerRadius * cos(lookingAngle + M_PI / 6), playerY + playerRadius * sin(lookingAngle + M_PI / 6));
        RenderMesh2D(meshes["circleEyes"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["circleEyesOutline"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(playerX + playerRadius * cos(lookingAngle - M_PI / 6), playerY + playerRadius * sin(lookingAngle - M_PI / 6));
        RenderMesh2D(meshes["circleEyes"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["circleEyesOutline"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(playerX, playerY);
        RenderMesh2D(meshes["circleBody"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["circleBodyOutline"], shaders["VertexColor"], modelMatrix);
    }

    // Pick Up Bonuses
    {
        for (int i = 0; i < bonuses.size(); i++) {
            bonusPickupObject bonusPickObj = bonuses[i];

            float distance_to_player = sqrt((playerX - bonusPickObj.obj_position.x) * (playerX - bonusPickObj.obj_position.x) + (playerY - bonusPickObj.obj_position.y) * (playerY - bonusPickObj.obj_position.y));

            if (distance_to_player <= 3 * playerRadius / 2) {
                if (bonusPickObj.type == 1) {
                    health += 40;

                    if (health > 100) {
                        health = 100;
                    }

                    cout << "Picked up health bonus" << endl;
                }

                if (bonusPickObj.type == 2) {
                    shutgunTime = 10;

                    cout << "Picked up shutgun bonus" << endl;
                }

                if (bonusPickObj.type == 3) {
                    machinegunTime = 10;

                    cout << "Picked up machine gun bonus" << endl;
                }
                

                bonuses.erase(bonuses.begin() + i);
                i--;
                continue;
            }

            if (bonusPickObj.type == 3) {
                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(0.33, 0.2);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);

                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y - 1.5 * bonusRadius / 3);
                modelMatrix *= transform2D::Scale(0.33, 0.2);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);

                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y + 1.5 * bonusRadius / 3);
                modelMatrix *= transform2D::Scale(0.33, 0.2);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);
            }

            if (bonusPickObj.type == 2) {
                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(0.2, 0.33);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);

                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x - 1.5 * bonusRadius / 3, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(0.2, 0.33);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);

                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x + 1.5 * bonusRadius / 3, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(0.2, 0.33);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);
            }

            if (bonusPickObj.type == 1) {
                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(0.33, 1);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);

                modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y);
                modelMatrix *= transform2D::Scale(1, 0.33);
                RenderMesh2D(meshes["healthBonusCross"], shaders["VertexColor"], modelMatrix);
            }

            modelMatrix = visMatrix * transform2D::Translate(bonusPickObj.obj_position.x, bonusPickObj.obj_position.y);
            RenderMesh2D(meshes["healthBonus"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Map
    {
        modelMatrix = visMatrix * transform2D::Translate(0, 0);
        modelMatrix *= transform2D::Scale(2,1);
        RenderMesh2D(meshes["background"], shaders["VertexColor"], modelMatrix);
    }
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W)) {
        if (playerY <= (20 - playerRadius - eyesRadius - 3 * outlineRadius / 4 - 3 * deltaTime) && !checkPlayerObstacleColision(playerX, playerY + 3 * deltaTime)) {
            logicSpace.y += 3 * deltaTime;
            playerY += 3 * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        if (playerY >= (playerRadius + eyesRadius + 3 * outlineRadius / 4 + 3 * deltaTime) && !checkPlayerObstacleColision(playerX, playerY - 3 * deltaTime)) {
            logicSpace.y -= 3 * deltaTime;
            playerY -= 3 * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        if (playerX >= (playerRadius + eyesRadius + 3 * outlineRadius / 4 + 3 * deltaTime) && !checkPlayerObstacleColision(playerX - 3 * deltaTime, playerY)) {
            logicSpace.x -= 3 * deltaTime;
            playerX -= 3 * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if (playerX <= (40 - playerRadius - eyesRadius - 3 * outlineRadius / 4 - 3 * deltaTime) && !checkPlayerObstacleColision(playerX + 3 * deltaTime, playerY)) {
            logicSpace.x += 3 * deltaTime;
            playerX += 3 * deltaTime;
        }
    }

    // TODO(student): Zoom in and zoom out logic window with Z and X
    if (window->KeyHold(GLFW_KEY_Z)) {
        logicSpace.width += 2 * deltaTime;
        logicSpace.height += 2 * deltaTime;
        logicSpace.x -= 1 * deltaTime;
        logicSpace.y -= 1 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_X)) {
        logicSpace.width -= 2 * deltaTime;
        logicSpace.height -= 2 * deltaTime;
        logicSpace.x += deltaTime;
        logicSpace.y += deltaTime;
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    this->mouseX = mouseX;
    this->mouseY = mouseY;

    glm::ivec2 resolution = window->GetResolution();

    lookingAngle = atan2(float(mouseX - resolution.x / 2) / (resolution.y / 2), float(mouseY - resolution.y / 2) / (resolution.y / 2)) - M_PI / 2;
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && machinegunTime > 0 && shutgunTime > 0 && (timeSinceStart - lastShot) >= 0.15) {
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle), playerY + playerRadius * sin(lookingAngle), 0), lookingAngle));
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle + M_PI / 6), playerY + playerRadius * sin(lookingAngle + M_PI / 6), 0), lookingAngle + M_PI / 6));
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle - M_PI / 6), playerY + playerRadius * sin(lookingAngle - M_PI / 6), 0), lookingAngle - M_PI / 6));
        lastShot = timeSinceStart;
    }

    else if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && machinegunTime > 0 && (timeSinceStart - lastShot) >= 0.15) {
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle), playerY + playerRadius * sin(lookingAngle), 0), lookingAngle));
        lastShot = timeSinceStart;
    }

    else if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && (timeSinceStart - lastShot) >= 0.5 && shutgunTime > 0) {
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle), playerY + playerRadius * sin(lookingAngle), 0), lookingAngle));
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle + M_PI / 6), playerY + playerRadius * sin(lookingAngle + M_PI / 6), 0), lookingAngle + M_PI / 6));
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle - M_PI / 6), playerY + playerRadius * sin(lookingAngle - M_PI / 6), 0), lookingAngle - M_PI / 6));
        lastShot = timeSinceStart;
    }

    // Add mouse button press event
    else  if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && (timeSinceStart - lastShot) >= 0.5) {
        bullets.push_back(Bullet(glm::vec3(playerX + playerRadius * cos(lookingAngle), playerY + playerRadius * sin(lookingAngle), 0), lookingAngle));
        lastShot = timeSinceStart;
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{

}
