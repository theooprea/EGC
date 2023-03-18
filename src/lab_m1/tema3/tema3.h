#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"

#include <random>

using namespace std;

namespace m1
{
    class Dancer {
    public:
        float row, col, angle, speed;
        Dancer(float row, float col, float angle, float speed) {
            this->row = row;
            this->col = col;
            this->angle = angle;
            this->speed = speed;
        }
    };

    class Reflector {
    public:
        glm::vec3 position, direction, colour;
        float spotlightOX, spotlightOY, speed;
        Reflector(glm::vec3 position, glm::vec3 direction, glm::vec3 colour, float spotlightOX, float spotlightOY, float speed) {
            this->position = position;
            this->direction = direction;
            this->colour = colour;
            this->spotlightOX = spotlightOX;
            this->spotlightOY = spotlightOY;
            this->speed = speed;
        }
    };

    class Tema3 : public gfxc::SimpleScene
    {
     public:
         Tema3();
        ~Tema3();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void Tema3::RenderUpperWallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int wall);
        void Tema3::RenderDiscoBallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));
        void RenderFloorMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));
        void RenderPlayerMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int row, int col);
        void RenderWallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int wall);
        Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void generateFloor() {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> colour_rand(0, 255);

                    float red = colour_rand(gen);
                    float green = colour_rand(gen);
                    float blue = colour_rand(gen);

                    floor[i][j] = glm::vec3(red / 255, green / 255, blue / 255);
                }
            }
        }

        void generateDancers() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> position_rand(100, 700);
            std::uniform_int_distribution<> angle_rand(0, 628);
            std::uniform_int_distribution<> speed_rand(5, 25);

            int nr_dancers_left = 10;

            while (nr_dancers_left > 0) {
                float row = (float)position_rand(gen) / 100;
                float col = (float)position_rand(gen) / 100;
                float angle = (float)angle_rand(gen) / 100;
                float speed = (float)speed_rand(gen) / 10;

                dancers.push_back(Dancer(row, col, angle, speed));

                nr_dancers_left--;
            }
        }

        void generateReflectors() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> colour_rand(0, 255);
            std::uniform_int_distribution<> spotlightOX_rand(50, 400);
            std::uniform_int_distribution<> spotlightOY_rand(0, 100);
            std::uniform_int_distribution<> speed_rand(5, 25);

            float red = colour_rand(gen);
            float green = colour_rand(gen);
            float blue = colour_rand(gen);
            float spotlightOX = spotlightOX_rand(gen);
            float spotlightOY = spotlightOY_rand(gen);
            float speed = speed_rand(gen);

            reflectors.push_back(Reflector(glm::vec3(1.5, 3.5, 1.5), glm::vec3(0, -1, 0), glm::vec3(red / 255, green / 255, blue / 255), spotlightOX / 1000, spotlightOY / 1000, speed / 10));

            red = colour_rand(gen);
            green = colour_rand(gen);
            blue = colour_rand(gen);
            spotlightOX = spotlightOX_rand(gen);
            spotlightOY = spotlightOY_rand(gen);
            speed = speed_rand(gen);

            reflectors.push_back(Reflector(glm::vec3(1.5, 3.5, 4.5), glm::vec3(0, -1, 0), glm::vec3(red / 255, green / 255, blue / 255), spotlightOX / 1000, spotlightOY / 1000, speed / 10));

            red = colour_rand(gen);
            green = colour_rand(gen);
            blue = colour_rand(gen);
            spotlightOX = spotlightOX_rand(gen);
            spotlightOY = spotlightOY_rand(gen);
            speed = speed_rand(gen);

            reflectors.push_back(Reflector(glm::vec3(4.5, 3.5, 1.5), glm::vec3(0, -1, 0), glm::vec3(red / 255, green / 255, blue / 255), spotlightOX / 1000, spotlightOY / 1000, speed / 10));

            red = colour_rand(gen);
            green = colour_rand(gen);
            blue = colour_rand(gen);
            spotlightOX = spotlightOX_rand(gen);
            spotlightOY = spotlightOY_rand(gen);
            speed = speed_rand(gen);

            reflectors.push_back(Reflector(glm::vec3(4.5, 3.5, 4.5), glm::vec3(0, -1, 0), glm::vec3(red / 255, green / 255, blue / 255), spotlightOX / 1000, spotlightOY / 1000, speed / 10));
        }

        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        std::unordered_map<std::string, Texture2D*> mapTextures;
        std::vector<Dancer> dancers;
        std::vector<Reflector> reflectors;

        float cutoffAngle;

        bool dancers_move;

        float spotlightOX;
        float spotlightOY;

        int light_type;

        glm::vec3 floor[8][8];
    };
}   // namespace m1
