#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/tema2_camera.h"
#include "lab_m1/tema2/projectiles.h"
#include "lab_m1/tema2/enemy.h"
#include <algorithm>
#include <array>
#include <random>
#include <chrono>
#include <time.h>
#include <queue>

namespace m1
{
    struct Point {
        int row;
        int col;
        int dist;
    };

    class Tema2 : public gfxc::SimpleScene
    {
     public:
         Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

        bool colisionBulletEnemy(glm::vec3 bullet_position, glm::vec3 enemy_position, float colision_distance) {
            return distance(bullet_position, enemy_position) <= colision_distance;
        }

        bool colisionLabirinthWalls(float objX, float objZ, float objDist) {
            for (int i = 0; i < labirinth_rows; i++) {
                for (int j = 0; j < labirinth_cols; j++) {
                    if (labirinth[i][j] == 1) {
                        glm::vec3 center = glm::vec3(i * 4 - 20, 1, j * 4 - 20);
                        glm::vec3 lowerLeft = center + glm::vec3(-2, 0, -2);
                        glm::vec3 lowerRight = center + glm::vec3(2, 0, -2);
                        glm::vec3 upperLeft = center + glm::vec3(-2, 0, 2);
                        glm::vec3 upperRight = center + glm::vec3(2, 0, 2);

                        if (objX >= lowerLeft.x && objX <= lowerRight.x && objZ >= lowerLeft.z - objDist && objZ < center.z) {
                            return true;
                        }

                        if (objX >= upperLeft.x && objX <= upperRight.x && objZ <= upperLeft.z + objDist && objZ > center.z) {
                            return true;
                        }

                        if (objZ >= lowerLeft.z && objZ <= upperLeft.z && objX >= lowerLeft.x - objDist && objX < center.x) {
                            return true;
                        }

                        if (objZ >= lowerRight.z && objZ <= upperRight.z && objX <= lowerRight.x + objDist && objX > center.x) {
                            return true;
                        }

                        glm::vec3 objPos = glm::vec3(objX, 1, objZ);
                        if (distance(lowerLeft, objPos) <= objDist || distance(lowerRight, objPos) <= objDist ||
                            distance(upperLeft, objPos) <= objDist || distance(upperRight, objPos) <= objDist) {
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        int distanceInLabirinth(int start_row, int start_col, int dest_row, int dest_col) {
            Point start = { start_row, start_col, 0 };

            bool visited[labirinth_rows][labirinth_cols];

            for (int i = 0; i < labirinth_rows; i++) {
                for (int j = 0; j < labirinth_cols; j++) {
                    visited[i][j] = false;
                }
            }

            std::queue<Point> queue;

            queue.push(start);

            while (!queue.empty()) {
                Point current = queue.front();
                queue.pop();

                // Neighbours
                int neigh_row, neigh_col;
                for (int i = 0; i < 4; i++) {
                    // North
                    if (i == 0) {
                        neigh_row = current.row - 1;
                        neigh_col = current.col;
                    }
                    // South
                    else if (i == 1) {
                        neigh_row = current.row + 1;
                        neigh_col = current.col;
                    }
                    // East
                    else if (i == 2) {
                        neigh_row = current.row;
                        neigh_col = current.col + 1;
                    }
                    // West
                    else {
                        neigh_row = current.row;
                        neigh_col = current.col - 1;
                    }

                    // If it is in grid
                    if (neigh_row >= 0 && neigh_row < labirinth_rows && neigh_col >= 0 && neigh_col < labirinth_cols) {
                        // If it is a hallway and wasn't visited
                        if (labirinth[neigh_row][neigh_col] == 0 && visited[neigh_row][neigh_col] == false) {
                            // If it's the destination
                            if (neigh_row == dest_row && neigh_col == dest_col) {
                                return current.dist + 1;
                            }
                            else {
                                visited[neigh_row][neigh_col] = true;
                                Point newPoint = { neigh_row, neigh_col, current.dist + 1 };
                                queue.push(newPoint);
                            }
                        }
                    }
                }
            }

            return -1;
        }

        void backtrackingGenerator(int current_row, int current_col) {
            std::array<Direction, 4> directions = { North, South, East, West };
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

            shuffle(directions.begin(), directions.end(), std::default_random_engine(seed));

            int new_row, new_col;

            for (int i = 0; i < 4; i++) {
                switch (directions[i])
                {
                case North:
                    new_row = current_row - 2;
                    new_col = current_col;
                    break;
                case South:
                    new_row = current_row + 2;
                    new_col = current_col;
                    break;
                case East:
                    new_row = current_row;
                    new_col = current_col + 2;
                    break;
                case West:
                    new_row = current_row;
                    new_col = current_col - 2;
                    break;
                default:
                    break;
                }

                // If the new position is valid (is on the grid)
                if (new_row >= 1 && new_row < labirinth_rows && new_col >= 1 && new_col < labirinth_cols) {
                    // If the new position was not visited
                    if (labirinth_generator_visited[new_row][new_col] == false) {
                        int passage_row = (new_row + current_row) / 2;
                        int passage_col = (new_col + current_col) / 2;

                        labirinth_generator_visited[new_row][new_col] = true;
                        labirinth[passage_row][passage_col] = 0;

                        backtrackingGenerator(new_row, new_col);
                    }
                }
            }
        }

        bool checkBossFightColision(float objX, float objZ, float objDist) {
            std::vector<glm::vec3> bossRoomWalls;

            // Check Barriers
            if (bossFight && bossDeathTime == 0) {
                bossRoomWalls.push_back(glm::vec3(bossRoomRow, 0, bossRoomCol - 2));
                bossRoomWalls.push_back(glm::vec3(bossRoomRow, 0, bossRoomCol + 2));
            }
            
            // Check BossRoom walls
            bossRoomWalls.push_back(glm::vec3(bossRoomRow - 2, 0, bossRoomCol));
            bossRoomWalls.push_back(glm::vec3(bossRoomRow + 2, 0, bossRoomCol));
            bossRoomWalls.push_back(glm::vec3(bossRoomRow - 1, 0, bossRoomCol + 1));
            bossRoomWalls.push_back(glm::vec3(bossRoomRow + 1, 0, bossRoomCol + 1));

            for (int i = 0; i < bossRoomWalls.size(); i++) {
                glm::vec3 wall = bossRoomWalls[i];

                glm::vec3 center = glm::vec3(wall.x * 4 - 20, 1, wall.z * 4 - 20);
                glm::vec3 lowerLeft = center + glm::vec3(-2, 0, -2);
                glm::vec3 lowerRight = center + glm::vec3(2, 0, -2);
                glm::vec3 upperLeft = center + glm::vec3(-2, 0, 2);
                glm::vec3 upperRight = center + glm::vec3(2, 0, 2);

                if (objX >= lowerLeft.x && objX <= lowerRight.x && objZ >= lowerLeft.z - objDist && objZ < center.z) {
                    return true;
                }

                if (objX >= upperLeft.x && objX <= upperRight.x && objZ <= upperLeft.z + objDist && objZ > center.z) {
                    return true;
                }

                if (objZ >= lowerLeft.z && objZ <= upperLeft.z && objX >= lowerLeft.x - objDist && objX < center.x) {
                    return true;
                }

                if (objZ >= lowerRight.z && objZ <= upperRight.z && objX <= lowerRight.x + objDist && objX > center.x) {
                    return true;
                }

                glm::vec3 objPos = glm::vec3(objX, 1, objZ);
                if (distance(lowerLeft, objPos) <= objDist || distance(lowerRight, objPos) <= objDist ||
                    distance(upperLeft, objPos) <= objDist || distance(upperRight, objPos) <= objDist) {
                    return true;
                }
            }

            return false;
        }

        void generateBossRoom() {
            bossRoomCol = labirinth_cols;
            bossRoomRow;

            for (int i = 0; i < labirinth_rows; i++) {
                if (labirinth[i][labirinth_cols - 1] == 0) {
                    bossRoomRow = i;
                    break;
                }
            }

            printf("%d %d\n", bossRoomRow, bossRoomCol);
        }

        void generateLabirinth() {
            // Setting initial labirinth
            for (int i = 0; i < labirinth_rows; i++) {
                for (int j = 0; j < labirinth_cols; j++) {
                    if (i % 2 == 0 || j % 2 == 0) {
                        labirinth[i][j] = 1;
                    }
                    else {
                        labirinth[i][j] = 0;
                    }
                }
            }

            // Setting visited matrix
            for (int i = 0; i < labirinth_rows; i++) {
                for (int j = 0; j < labirinth_cols; j++) {
                    if (i % 2 == 1 && j % 2 == 1) {
                        labirinth_generator_visited[i][j] = false;
                    }
                }
            }
            
            // Randomly selecting the starting point
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> startRand(1, (labirinth_rows - 1) / 2);

            starting_pos_row = startRand(gen) * 2 - 1;

            // Generating the maze
            backtrackingGenerator(starting_pos_row, 1);

            labirinth[starting_pos_row][0] = 0;

            // Choosing the most distant cell to mark as the exit
            int distMax = 0;
            int exit_row;
            int exit_col = labirinth_cols - 2;
            for (int i = 0; i < labirinth_rows; i++) {
                if (labirinth[i][labirinth_cols - 2] == 0) {
                    int distCurrent = distanceInLabirinth(i, labirinth_cols - 2, starting_pos_row, 1);
                    if (distCurrent > distMax) {
                        distMax = distCurrent;
                        exit_row = i;
                    }
                }
            }

            labirinth[exit_row][labirinth_cols - 1] = 0;

            // Randomly placing the enemies
            std::uniform_int_distribution<> labirinth_position_randomizer(1, (labirinth_rows - 2));
            std::uniform_int_distribution<> enemy_starting_point_randomizer(1, 4);

            int remaining_enemies = enemies_number;
            while (remaining_enemies > 0) {
                int enemy_row = labirinth_position_randomizer(gen);
                int enemy_col = labirinth_position_randomizer(gen);

                if (labirinth[enemy_row][enemy_col] == 0) {
                    labirinth[enemy_row][enemy_col] = 2;
                    int initialPos = enemy_starting_point_randomizer(gen);

                    switch (initialPos)
                    {
                    case 1:
                        enemies.push_back(EnemyLabirinth(glm::vec3(4 * enemy_row - 20, 1, 4 * enemy_col - 20), glm::vec3(4 * enemy_row - 20 - 1.5, 1, 4 * enemy_col - 20 - 1.5), Down, shaders["LabShader"]));
                        break;
                    case 2:
                        enemies.push_back(EnemyLabirinth(glm::vec3(4 * enemy_row - 20, 1, 4 * enemy_col - 20), glm::vec3(4 * enemy_row - 20 + 1.5, 1, 4 * enemy_col - 20 - 1.5), Right, shaders["LabShader"]));
                        break;
                    case 3:
                        enemies.push_back(EnemyLabirinth(glm::vec3(4 * enemy_row - 20, 1, 4 * enemy_col - 20), glm::vec3(4 * enemy_row - 20 + 1.5, 1, 4 * enemy_col - 20 + 1.5), Up, shaders["LabShader"]));
                        break;
                    case 4:
                        enemies.push_back(EnemyLabirinth(glm::vec3(4 * enemy_row - 20, 1, 4 * enemy_col - 20), glm::vec3(4 * enemy_row - 20 - 1.5, 1, 4 * enemy_col - 20 + 1.5), Left, shaders["LabShader"]));
                        break;
                    default:
                        break;
                    }

                    remaining_enemies--;
                }
            }
        }

        bool playerInCell(glm::vec3 cell_center_position, float objDist) {
            glm::vec3 lowerLeft = cell_center_position + glm::vec3(-2, 0, -2);
            glm::vec3 lowerRight = cell_center_position + glm::vec3(2, 0, -2);
            glm::vec3 upperLeft = cell_center_position + glm::vec3(-2, 0, 2);
            glm::vec3 upperRight = cell_center_position + glm::vec3(2, 0, 2);

            if (playerX >= lowerLeft.x && playerX <= lowerRight.x && playerZ >= lowerLeft.z - objDist && playerZ < cell_center_position.z) {
                return true;
            }

            if (playerX >= upperLeft.x && playerX <= upperRight.x && playerZ <= upperLeft.z + objDist && playerZ > cell_center_position.z) {
                return true;
            }

            if (playerZ >= lowerLeft.z && playerZ <= upperLeft.z && playerX >= lowerLeft.x - objDist && playerX < cell_center_position.x) {
                return true;
            }

            if (playerZ >= lowerRight.z && playerZ <= upperRight.z && playerX <= lowerRight.x + objDist && playerX > cell_center_position.x) {
                return true;
            }

            return false;
        }

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float colisionTime);
        void Tema2::RenderSun(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, Texture2D* texture);

     protected:
        implemented::CameraTema2 *camera, *camera1, *camera2;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float field_of_view;
        int projection_type;
        float playerX, playerY, playerZ;

        float left = -10.0f, right = 10.0f, bottom = -10.0f, top = 10.0f;

        // TODO(student): If you need any other class variables, define them here.

        glm::vec3 lightPosition;

        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        float playerAngleLeftRight;
        float playerAngleUpDown;
        float timeSinceStart;
        float timeLeft;
        float lastShot;
        float health;

        // Bonus Variables
        float bossHealth;
        float bossDeathTime, bossDyingTimeLeft;
        bool bossFight, gameOver, gameOverPrinted;
        
        float limbsAngle;
        bool limbsDirection;

        std::vector<Projectile> projectiles;
        std::vector<EnemyLabirinth> enemies;
        std::unordered_map<std::string, Texture2D*> mapTextures;

        static const int labirinth_rows = 11;
        static const int labirinth_cols = 11;
        static const int enemies_number = 8;
        int labirinth[labirinth_rows][labirinth_cols];
        bool labirinth_generator_visited[labirinth_rows][labirinth_cols];

        int starting_pos_row, bossRoomRow, bossRoomCol;

        enum Direction {North, South, East, West};
    };
}   // namespace m1
