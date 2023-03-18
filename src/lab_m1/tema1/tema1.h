#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema1/obstacle.h"
#include "lab_m1/tema1/enemy.h"
#include "lab_m1/tema1/bullet.h"
#include "lab_m1/tema1/bonusPickupObject.h"
#include <iostream>
#include <random>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        bool checkBulletObstacle(float pozX, float pozY) {
            for (Obstacle& obstacle : obstacles) {
                if (pozX >= obstacle.lower_left.x - playerRadius / 8 && pozX <= obstacle.lower_right.x + playerRadius / 8 && pozY <= obstacle.upper_left.y + playerRadius / 8 && pozY >= obstacle.lower_left.y - playerRadius / 8) {
                    return true;
                }
            }
            return false;
        }

        // TODO update
        bool checkBulletEnemy(Enemy enemy, Bullet bullet, float angle_to_player) {
            // hands
            glm::vec3 leftHand(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player + M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player + M_PI / 4), 0);
            glm::vec3 rightHand(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player - M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player - M_PI / 4), 0);

            // left hand corners
            glm::vec3 leftHandleftCorner(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player + M_PI / 4) + (playerRadius / 2) * sqrt(2) * cos(angle_to_player + M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player + M_PI / 4) + (playerRadius / 2) * sqrt(2) * sin(angle_to_player + M_PI / 4), 0);
            glm::vec3 leftHandrightCorner(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player + M_PI / 4) + (playerRadius / 2) * sqrt(2) * cos(angle_to_player - M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player + M_PI / 4) + +(playerRadius / 2) * sqrt(2) * sin(angle_to_player - M_PI / 4), 0);

            // right hand corners
            glm::vec3 rightHandleftCorner(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player - M_PI / 4) + (playerRadius / 2) * sqrt(2) * cos(angle_to_player + M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player - M_PI / 4) + (playerRadius / 2) * sqrt(2) * sin(angle_to_player + M_PI / 4), 0);
            glm::vec3 rightHandrightCorner(enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle_to_player - M_PI / 4) + (playerRadius / 2) * sqrt(2) * cos(angle_to_player - M_PI / 4), enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle_to_player - M_PI / 4) + (playerRadius / 2) * sqrt(2) * sin(angle_to_player - M_PI / 4), 0);

            // check for frontal hit
            if (sqrt((enemy.enemy_position.x - bullet.bullet_position.x) * (enemy.enemy_position.x - bullet.bullet_position.x) + (enemy.enemy_position.y - bullet.bullet_position.y) * (enemy.enemy_position.y - bullet.bullet_position.y)) <= playerRadius + (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // left hand
            if (sqrt((leftHand.x - bullet.bullet_position.x) * (leftHand.x - bullet.bullet_position.x) + (leftHand.y - bullet.bullet_position.y) * (leftHand.y - bullet.bullet_position.y)) <= playerRadius / 2 + (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // right hand
            if (sqrt((rightHand.x - bullet.bullet_position.x) * (rightHand.x - bullet.bullet_position.x) + (rightHand.y - bullet.bullet_position.y) * (rightHand.y - bullet.bullet_position.y)) <= playerRadius / 2 + (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // left hand left corner
            if (sqrt((leftHandleftCorner.x - bullet.bullet_position.x) * (leftHandleftCorner.x - bullet.bullet_position.x) + (leftHandleftCorner.y - bullet.bullet_position.y) * (leftHandleftCorner.y - bullet.bullet_position.y)) <= (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // left hand right corner
            if (sqrt((leftHandrightCorner.x - bullet.bullet_position.x) * (leftHandrightCorner.x - bullet.bullet_position.x) + (leftHandrightCorner.y - bullet.bullet_position.y) * (leftHandrightCorner.y - bullet.bullet_position.y)) <= (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // right hand left corner
            if (sqrt((rightHandleftCorner.x - bullet.bullet_position.x) * (rightHandleftCorner.x - bullet.bullet_position.x) + (rightHandleftCorner.y - bullet.bullet_position.y) * (rightHandleftCorner.y - bullet.bullet_position.y)) <= (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            // right hand right corner
            if (sqrt((rightHandrightCorner.x - bullet.bullet_position.x) * (rightHandrightCorner.x - bullet.bullet_position.x) + (rightHandrightCorner.y - bullet.bullet_position.y) * (rightHandrightCorner.y - bullet.bullet_position.y)) <= (bulletSize / 2) * sqrt(2)) {
                return true;
            }

            return false;
        }

        bool checkPlayerEnemyColision(float playerXArg, float playerYArg, Enemy enemy, float angle) {
            glm::vec3 pozLeftHand;
            pozLeftHand.x = enemy.enemy_position.x + playerRadius * sqrt(2) * cos(angle + M_PI / 4);
            pozLeftHand.y = enemy.enemy_position.y + playerRadius * sqrt(2) * sin(angle + M_PI / 4);

            if (sqrt((pozLeftHand.x - playerXArg) * (pozLeftHand.x - playerXArg) + (pozLeftHand.y - playerYArg) * (pozLeftHand.y - playerYArg)) <= playerRadius + eyesRadius + ((playerRadius + outlineRadius) / 2) * sqrt(2)) {
                return true;
            }

            return false;
        }

        bool checkPlayerObstacleColision(float playerXArg, float playerYArg) {
            for (Obstacle& obstacle : obstacles) {
                if (playerXArg <= obstacle.lower_right.x && playerXArg >= obstacle.lower_left.x && playerYArg >= obstacle.lower_right.y - playerRadius - eyesRadius - 3 * outlineRadius / 4 && playerYArg < obstacle.lower_right.y) {
                    return true;
                }

                if (playerXArg <= obstacle.upper_right.x && playerXArg >= obstacle.upper_left.x && playerYArg <= obstacle.upper_right.y + playerRadius + eyesRadius + 3 * outlineRadius / 4 && playerYArg > obstacle.upper_right.y) {
                    return true;
                }

                if (playerYArg <= obstacle.upper_left.y && playerYArg >= obstacle.lower_left.y && playerXArg >= obstacle.lower_left.x - playerRadius - eyesRadius - 3 * outlineRadius / 4 && playerXArg < obstacle.lower_left.x) {
                    return true;
                }

                if (playerYArg <= obstacle.upper_right.y && playerYArg >= obstacle.lower_right.y && playerXArg <= obstacle.upper_right.x + playerRadius + eyesRadius + 3 * outlineRadius / 4 && playerXArg > obstacle.upper_right.x) {
                    return true;
                }

                if (sqrt((playerXArg - obstacle.lower_left.x) * (playerXArg - obstacle.lower_left.x) + (playerYArg - obstacle.lower_left.y) * (playerYArg - obstacle.lower_left.y)) <= playerRadius + eyesRadius + 3 * outlineRadius / 4) {
                    return true;
                }

                if (sqrt((playerXArg - obstacle.lower_right.x) * (playerXArg - obstacle.lower_right.x) + (playerYArg - obstacle.lower_right.y) * (playerYArg - obstacle.lower_right.y)) <= playerRadius + eyesRadius + 3 * outlineRadius / 4) {
                    return true;
                }

                if (sqrt((playerXArg - obstacle.upper_left.x) * (playerXArg - obstacle.upper_left.x) + (playerYArg - obstacle.upper_left.y) * (playerYArg - obstacle.upper_left.y)) <= playerRadius + eyesRadius + 3 * outlineRadius / 4) {
                    return true;
                }

                if (sqrt((playerXArg - obstacle.upper_right.x) * (playerXArg - obstacle.upper_right.x) + (playerYArg - obstacle.upper_right.y) * (playerYArg - obstacle.upper_right.y)) <= playerRadius + eyesRadius + 3 * outlineRadius / 4) {
                    return true;
                }
            }
            return false;
        }

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

     protected:
        float length;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        LogicSpace healthBarSpace;
        glm::mat3 modelMatrix, visMatrix;

        float playerX, playerY;
        float playerRadius;
        double mouseX, mouseY;

        float lookingAngle;
        float timeSinceStart;
        float lastShot;
        float lastEnemySpanned;
        float eyesRadius;
        float bulletSize;
        float health;
        float outlineRadius;
        float bonusRadius;
        float shutgunTime;
        float machinegunTime;
        float bonusPickUpTime;
        int score;

        std::vector<Obstacle> obstacles;
        std::vector<Enemy> enemies;
        std::vector<Bullet> bullets;
        std::vector<Bullet> enemyBullets;
        std::vector<bonusPickupObject> bonuses;
    };
}   // namespace m1
