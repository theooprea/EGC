#include "lab_m1/tema3/tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    generateFloor();
    generateDancers();
    generateReflectors();

    dancers_move = true;

    light_type = 0;

    // Create a simple cube
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0, 0)),
            VertexFormat(glm::vec3(1, -1, 0)),
        };

        vector<unsigned int> indices =
        {
        };

        float angle = 2 * M_PI / 100;
        for (int i = 2; i < 100; i++) {
            vertices.push_back(VertexFormat(glm::vec3(cos(angle * (i - 1)), -1, sin(angle * (i - 1)))));
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i - 1);
        }

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(99);

        CreateMesh("con", vertices, indices);
    }

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
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("FloorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShaderFloor.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShaderFloor.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("PlayerShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShaderPlayer.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShaderPlayer.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("ConShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShaderCon.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShaderCon.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("DiscoBallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShaderDiscoBall.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShaderDiscoBall.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("UpperWallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShaderUpperWall.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShaderUpperWall.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 1, 1);
        lightDirection = glm::vec3(0, -1, 0);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
        cutoffAngle =  M_PI / 9;
        spotlightOX = 0;
        spotlightOY = 0;
    }

    {
        mapTextures["random"] = CreateRandomTexture(16, 16);
    }
}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    for (int i = 0; i < 4; i++) {
        Reflector* reflector = &reflectors[i];

        reflector->spotlightOY += reflector->speed * deltaTimeSeconds;

        glm::mat4 lightRotation = glm::mat4(1);
        lightRotation = glm::rotate(lightRotation, reflector->spotlightOY, glm::vec3(0, 1, 0));
        lightRotation = glm::rotate(lightRotation, reflector->spotlightOX, glm::vec3(1, 0, 0));

        reflector->direction = glm::vec3(lightRotation * glm::vec4(0, -1, 0, 0));
    }

    // Render ground
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3((float)i * 3 / 4, 0, (float)j * 3/ 4));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75, 0.1, 0.75));

                RenderFloorMesh(meshes["box"], shaders["FloorShader"], modelMatrix, floor[i][j]);
            }
        }
    }

    // Render walls
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 1.75, 3 - 0.375));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 3.5, 6));

        RenderWallMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), 0);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(6 - 0.35, 1.75, 3 - 0.375));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 3.5, 6));

        RenderWallMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), 2);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3 - 0.375, 1.75, -0.4));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 3.5, 0.05));

        RenderWallMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), 1);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3 - 0.375, 1.75, 6 - 0.35));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 3.5, 0.05));

        RenderWallMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), 3);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3 - 0.375, 3.5, 3 - 0.375));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 0.05, 6));

        RenderUpperWallMesh(meshes["box"], shaders["UpperWallShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), 0);
    }

    // Dancers
    {
        for (int i = 0; i < dancers.size(); i++) {
            Dancer* dancer = &dancers[i];

            if (dancers_move) {
                if (dancer->row + cos(dancer->angle) * dancer->speed * deltaTimeSeconds < 1 || dancer->row + cos(dancer->angle) * dancer->speed * deltaTimeSeconds > 7 ||
                    dancer->col + sin(dancer->angle) * dancer->speed * deltaTimeSeconds < 1 || dancer->col + sin(dancer->angle) * dancer->speed * deltaTimeSeconds > 7) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> angle_rand(0, 628);

                    dancer->angle += (float)angle_rand(gen) / 100;
                }
                else {
                    dancer->row = dancer->row + cos(dancer->angle) * dancer->speed * deltaTimeSeconds;
                    dancer->col = dancer->col + sin(dancer->angle) * dancer->speed * deltaTimeSeconds;
                }
            }

            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(dancer->col * 3 / 4 - 0.375, 0.375, dancer->row * 3 / 4 - 0.375));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25, 0.75, 0.25));

            RenderPlayerMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.4), (int)dancer->col, (int)dancer->row);
        }
    }

    // Disco ball
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3, 3, 3));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1));

        RenderDiscoBallMesh(meshes["sphere"], shaders["DiscoBallShader"], modelMatrix, mapTextures["random"]);
    }

    // Render the point light in the scene
    if (light_type == 0 || light_type == 2) {
        glm::mat4 modelMatrix = glm::mat4(1);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glDepthMask(GL_FALSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = 0; i < reflectors.size(); i++) {
            Reflector* reflector = &reflectors[i];

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, reflector->position);

            modelMatrix = glm::rotate(modelMatrix, reflector->spotlightOY, glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, reflector->spotlightOX, glm::vec3(1, 0, 0));

            modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffAngle) * 3.5, 4.5, tan(cutoffAngle) * 3.5));

            RenderFloorMesh(meshes["con"], shaders["ConShader"], modelMatrix, reflector->colour);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }
}


void Tema3::FrameEnd()
{

}

void Tema3::RenderUpperWallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int wall)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glm::vec3 pointLightPos[9];
    glm::vec3 pointLightColor[9];

    {
        if (wall == 0) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3(0, 0.05, (float)i * 3 / 4);
                pointLightColor[i] = floor[0][i];
            }
        }
        else if (wall == 1) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3((float)i * 3 / 4, 0.05, 0);
                pointLightColor[i] = floor[i][0];
            }
        }
        else if (wall == 2) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3(7.0f * 3 / 4, 0.05, (float)i * 3 / 4);
                pointLightColor[i] = floor[7][i];
            }
        }
        else if (wall == 3) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3((float)i * 3 / 4, 0.05, 7.0f * 3 / 4);
                pointLightColor[i] = floor[i][7];
            }
        }
        else {

        }
    }

    // Set shader uniforms for light properties
    int light_positions = glGetUniformLocation(shader->program, "light_positions");

    glUniform3fv(light_positions, 9, glm::value_ptr(pointLightPos[0]));

    // Set shader uniforms for light properties
    int light_colours = glGetUniformLocation(shader->program, "light_colours");
    glUniform3fv(light_colours, 9, glm::value_ptr(pointLightColor[0]));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int disco_ball_position = glGetUniformLocation(shader->program, "disco_ball_position");
    glUniform3f(disco_ball_position, 3, 3, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

    int locTime = glGetUniformLocation(shader->program, "time");
    glUniform1f(locTime, (GLfloat)Engine::GetElapsedTime());

    int disco_ball = glGetUniformLocation(shader->program, "disco_ball");

    if (light_type == 0 || light_type == 3) {
        glUniform1i(disco_ball, 1);
    }
    else {
        glUniform1i(disco_ball, 0);
    }
    
    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderWallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int wall)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glm::vec3 pointLightPos[9];
    glm::vec3 pointLightColor[9];

    {
        if (wall == 0) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3(0, 0.05, (float)i * 3 / 4);
                pointLightColor[i] = floor[0][i];
            }
        }
        else if (wall == 1) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3((float)i * 3 / 4, 0.05, 0);
                pointLightColor[i] = floor[i][0];
            }
        }
        else if (wall == 2) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3(7.0f * 3 / 4, 0.05, (float)i * 3 / 4);
                pointLightColor[i] = floor[7][i];
            }
        }
        else if (wall == 3) {
            for (int i = 0; i < 8; i++) {
                pointLightPos[i] = glm::vec3((float)i * 3 / 4, 0.05, 7.0f * 3 / 4);
                pointLightColor[i] = floor[i][7];
            }
        }
        else {

        }
    }

    // Set shader uniforms for light properties
    int light_positions = glGetUniformLocation(shader->program, "light_positions");

    glUniform3fv(light_positions, 9, glm::value_ptr(pointLightPos[0]));

    // Set shader uniforms for light properties
    int light_colours = glGetUniformLocation(shader->program, "light_colours");
    glUniform3fv(light_colours, 9, glm::value_ptr(pointLightColor[0]));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int disco_ball_position = glGetUniformLocation(shader->program, "disco_ball_position");
    glUniform3f(disco_ball_position, 3, 3, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

    int locTime = glGetUniformLocation(shader->program, "time");
    glUniform1f(locTime, (GLfloat)Engine::GetElapsedTime());

    int disco_ball = glGetUniformLocation(shader->program, "disco_ball");

    if (light_type == 0 || light_type == 3) {
        glUniform1i(disco_ball, 1);
    }
    else {
        glUniform1i(disco_ball, 0);
    }

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderPlayerMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int row, int col)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glm::vec3 pointLightPos[9];
    glm::vec3 pointLightColor[9];

    glm::vec3 pointLightPosSpot[4];
    glm::vec3 pointLightDirSpot[4];
    glm::vec3 pointLightColSpot[4];

    if (light_type == 0 || light_type == 2) {
        for (int i = 0; i < 4; i++) {
            pointLightPosSpot[i] = reflectors[i].position;
            pointLightDirSpot[i] = reflectors[i].direction;
            pointLightColSpot[i] = reflectors[i].colour;
        }
    }

    if (light_type == 0 || light_type == 1) {
        pointLightPos[0] = glm::vec3((float)row * 3 / 4, 0.05, (float)col * 3 / 4);

        pointLightPos[1] = glm::vec3((float)row * 3 / 4, 0.05, (float)(col + 1) * 3 / 4);
        pointLightPos[2] = glm::vec3((float)(row - 1) * 3 / 4, 0.05, (float)(col + 1) * 3 / 4);
        pointLightPos[3] = glm::vec3((float)(row - 1) * 3 / 4, 0.05, (float)col * 3 / 4);

        pointLightPos[4] = glm::vec3((float)(row - 1) * 3 / 4, 0.05, (float)(col - 1) * 3 / 4);
        pointLightPos[5] = glm::vec3((float)row * 3 / 4, 0.05, (float)(col - 1) * 3 / 4);
        pointLightPos[6] = glm::vec3((float)(row + 1) * 3 / 4, 0.05, (float)(col - 1) * 3 / 4);

        pointLightPos[7] = glm::vec3((float)(row + 1) * 3 / 4, 0.05, (float)col * 3 / 4);
        pointLightPos[8] = glm::vec3((float)(row + 1) * 3 / 4, 0.05, (float)(col + 1) * 3 / 4);

        pointLightColor[0] = floor[row][col];

        pointLightColor[1] = floor[row][col + 1];
        pointLightColor[2] = floor[row - 1][col + 1];
        pointLightColor[3] = floor[row - 1][col];

        pointLightColor[4] = floor[row - 1][col - 1];
        pointLightColor[5] = floor[row][col - 1];
        pointLightColor[6] = floor[row + 1][col - 1];

        pointLightColor[7] = floor[row + 1][col];
        pointLightColor[8] = floor[row + 1][col + 1];
    }


    int light_positions = glGetUniformLocation(shader->program, "light_positions");
    glUniform3fv(light_positions, 9, glm::value_ptr(pointLightPos[0]));

    int light_colours = glGetUniformLocation(shader->program, "light_colours");
    glUniform3fv(light_colours, 9, glm::value_ptr(pointLightColor[0]));

    int light_directions_spotlight = glGetUniformLocation(shader->program, "light_directions_spotlight");
    glUniform3fv(light_directions_spotlight, 4, glm::value_ptr(pointLightDirSpot[0]));

    int light_positions_spotlight = glGetUniformLocation(shader->program, "light_positions_spotlight");
    glUniform3fv(light_positions_spotlight, 4, glm::value_ptr(pointLightPosSpot[0]));

    int light_colours_spotlight = glGetUniformLocation(shader->program, "light_colours_spotlight");
    glUniform3fv(light_colours_spotlight, 4, glm::value_ptr(pointLightColSpot[0]));

    int cut_off_angle = glGetUniformLocation(shader->program, "cutoff_angle");
    glUniform1f(cut_off_angle, cutoffAngle);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    int disco_ball_position = glGetUniformLocation(shader->program, "disco_ball_position");
    glUniform3f(disco_ball_position, 3, 3, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

    int locTime = glGetUniformLocation(shader->program, "time");
    glUniform1f(locTime, (GLfloat)Engine::GetElapsedTime());

    int disco_ball = glGetUniformLocation(shader->program, "disco_ball");

    if (light_type == 0 || light_type == 3) {
        glUniform1i(disco_ball, 1);
    }
    else {
        glUniform1i(disco_ball, 0);
    }

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderFloorMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glm::vec3 pointLightPos[4];
    glm::vec3 pointLightDir[4];
    glm::vec3 pointLightColor[4];

    if (light_type == 0 || light_type == 2) {
        for (int i = 0; i < 4; i++) {
            pointLightPos[i] = reflectors[i].position;
            pointLightDir[i] = reflectors[i].direction;
            pointLightColor[i] = reflectors[i].colour;
        }
    }

    // Set shader uniforms for light properties
    int light_positions = glGetUniformLocation(shader->program, "light_positions");
    glUniform3fv(light_positions, 4, glm::value_ptr(pointLightPos[0]));

    int light_directions = glGetUniformLocation(shader->program, "light_directions");
    glUniform3fv(light_directions, 4, glm::value_ptr(pointLightDir[0]));

    // Set shader uniforms for light properties
    int light_colours = glGetUniformLocation(shader->program, "light_colours");
    glUniform3fv(light_colours, 4, glm::value_ptr(pointLightColor[0]));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    int disco_ball_position = glGetUniformLocation(shader->program, "disco_ball_position");
    glUniform3f(disco_ball_position, 3, 3, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

    int locTime = glGetUniformLocation(shader->program, "time");
    glUniform1f(locTime, (GLfloat)Engine::GetElapsedTime());

    int disco_ball = glGetUniformLocation(shader->program, "disco_ball");

    if (light_type == 0 || light_type == 3) {
        glUniform1i(disco_ball, 1);
    }
    else {
        glUniform1i(disco_ball, 0);
    }

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int cut_off_angle = glGetUniformLocation(shader->program, "cutoff_angle");
    glUniform1f(cut_off_angle, cutoffAngle);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

    int light_direction = glGetUniformLocation(shader->program, "light_direction");
    glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int cut_off_angle = glGetUniformLocation(shader->program, "cutoff_angle");
    glUniform1f(cut_off_angle, cutoffAngle);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderDiscoBallMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Set any other shader uniforms that you need
    int locTime = glGetUniformLocation(shader->program, "time");
    glUniform1f(locTime, (GLfloat)Engine::GetElapsedTime());

    if (texture)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_uniform"), 0);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];

    // TODO(student): Generate random texture data
    for (size_t i = 0; i < size; ++i) {
        data[i] = rand() % (UINT8_MAX + 1);
    }

    // TODO(student): Generate and bind the new texture ID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps
    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}

Mesh* Tema3::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
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

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // TODO(student): If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

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


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

        // Control light position using on W, A, S, D, E, Q
        if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * speed;

        // TODO(student): Set any other keys that you might need
        if (window->KeyHold(GLFW_KEY_Z)) {
            cutoffAngle = (cutoffAngle + deltaTime) < M_PI ? (cutoffAngle + deltaTime) : M_PI;
        }

        if (window->KeyHold(GLFW_KEY_X)) {
            cutoffAngle = (cutoffAngle - deltaTime) > 0 ? (cutoffAngle - deltaTime) : 0;
        }

        if (window->KeyHold(GLFW_KEY_UP)) {
            spotlightOX += deltaTime * speed;
        }

        if (window->KeyHold(GLFW_KEY_DOWN)) {
            spotlightOX -= deltaTime * speed;
        }

        if (window->KeyHold(GLFW_KEY_LEFT)) {
            spotlightOY += deltaTime * speed;
        }
        
        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            spotlightOY -= deltaTime * speed;
        }
    }
}


void Tema3::OnKeyPress(int key, int mods)
{
    // Add key press event

    if (key == GLFW_KEY_M) {
        dancers_move = !dancers_move;
    }

    if (key == GLFW_KEY_P) {
        light_type = (light_type + 1) % 4;
    }
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
