#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void renderSphere(unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS);
void init_particles_position();
glm::vec3 move_particle(int particle_number, float unit);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// particles - settings
const unsigned int PARTICLES_NUMBER = 2000;
std::vector<glm::vec3> particles_position(PARTICLES_NUMBER);
bool init_position = false;
bool init_position_press = false;
bool initialized = false;
float rotation_angle_particle_system_y = 0.0f;
float rotation_angle_particle_system_x = 0.0f;
float rotation_angle_particle_system_z = 0.0f;

// camera
glm::vec3 camera_position(0.0f, 0.0f, 4.0f);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
float rotation_angle_lamp_y = 0.0f;
float rotation_angle_lamp_x = 0.0f;
float rotation_angle_lamp_z = 0.0f;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader("light_casters.vs", "light_casters.fs");
    Shader lamp_shader("vertex_shader_lamp.vs", "fragment_shader_lamp.fs");

    float cube_vertices[] = {
        // positions            //normals
        -0.2f, -0.2f, -0.2f,    0.0f,  0.0f, -1.0f,
         0.2f, -0.2f, -0.2f,    0.0f,  0.0f, -1.0f,
         0.2f,  0.2f, -0.2f,    0.0f,  0.0f, -1.0f,
         0.2f,  0.2f, -0.2f,    0.0f,  0.0f, -1.0f,
        -0.2f,  0.2f, -0.2f,    0.0f,  0.0f, -1.0f,
        -0.2f, -0.2f, -0.2f,    0.0f,  0.0f, -1.0f,

        -0.2f, -0.2f,  0.2f,    0.0f,  0.0f,  1.0f,
         0.2f, -0.2f,  0.2f,    0.0f,  0.0f,  1.0f,
         0.2f,  0.2f,  0.2f,    0.0f,  0.0f,  1.0f,
         0.2f,  0.2f,  0.2f,    0.0f,  0.0f,  1.0f,
        -0.2f,  0.2f,  0.2f,    0.0f,  0.0f,  1.0f,
        -0.2f, -0.2f,  0.2f,    0.0f,  0.0f,  1.0f,

        -0.2f,  0.2f,  0.2f,   -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f, -0.2f,   -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f,   -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f, -0.2f,   -1.0f,  0.0f,  0.0f,
        -0.2f, -0.2f,  0.2f,   -1.0f,  0.0f,  0.0f,
        -0.2f,  0.2f,  0.2f,   -1.0f,  0.0f,  0.0f,

         0.2f,  0.2f,  0.2f,    1.0f,  0.0f,  0.0f,
         0.2f,  0.2f, -0.2f,    1.0f,  0.0f,  0.0f,
         0.2f, -0.2f, -0.2f,    1.0f,  0.0f,  0.0f,
         0.2f, -0.2f, -0.2f,    1.0f,  0.0f,  0.0f,
         0.2f, -0.2f,  0.2f,    1.0f,  0.0f,  0.0f,
         0.2f,  0.2f,  0.2f,    1.0f,  0.0f,  0.0f,

        -0.2f, -0.2f, -0.2f,    0.0f, -1.0f,  0.0f,
         0.2f, -0.2f, -0.2f,    0.0f, -1.0f,  0.0f,
         0.2f, -0.2f,  0.2f,    0.0f, -1.0f,  0.0f,
         0.2f, -0.2f,  0.2f,    0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f,  0.2f,    0.0f, -1.0f,  0.0f,
        -0.2f, -0.2f, -0.2f,    0.0f, -1.0f,  0.0f,

        -0.2f,  0.2f, -0.2f,    0.0f,  1.0f,  0.0f,
         0.2f,  0.2f, -0.2f,    0.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.2f,    0.0f,  1.0f,  0.0f,
         0.2f,  0.2f,  0.2f,    0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f,  0.2f,    0.0f,  1.0f,  0.0f,
        -0.2f,  0.2f, -0.2f,    0.0f,  1.0f,  0.0f
    };

    unsigned int wired_cube_indices[] = {
        0, 1,
        1, 2,
        2 ,4,
        4, 0,

        6, 7,
        7, 8,
        8, 10,
        10, 6,

        12, 13,
        13, 14,
        14, 16,
        16, 12,

        18, 19,
        19, 20, 
        20, 22,
        22, 18,

        24, 25,
        25, 26,
        26, 28,
        28, 24,

        30, 31,
        31, 32,
        32, 34,
        34, 30
    };

    glm::vec3 cube_position(0.0f, 0.0f, 1.0f);

    //CARICAMENTO DEI MODELLI 3D.
    Model lamp_model("../models/lamp/lamp.obj");

    unsigned int cubeVBO, cubeVAO, cubeEBO;

    glGenVertexArrays(1, &cubeVAO);

    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wired_cube_indices), wired_cube_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    init_particles_position();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("light.direction", -lightPos);
        ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("viewPos", camera_position);

        ourShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("light.constant", 1.0f);
        ourShader.setFloat("light.linear", 0.09f);
        ourShader.setFloat("light.quadratic", 0.032f);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(camera_position, camera_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); 
        ourShader.setMat4("model", model);

        ourShader.setVec3("material.specular", glm::vec3(0.5f));
        ourShader.setFloat("material.shininess", 84.0f);

        ourShader.setVec3("material.ambient", glm::vec3(0.5f));
        ourShader.setVec3("material.diffuse", glm::vec3(0.5f));

        if (!init_position)
            init_particles_position();

        for (int i = 0; i < PARTICLES_NUMBER; i++)
        {
            glm::vec3 tmp = glm::vec3(0.0f);

            if (init_position)
                tmp = move_particle(i, 0.01f);

            model = glm::mat4(1.0f);
            model = glm::rotate(model, rotation_angle_particle_system_y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation_angle_particle_system_x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation_angle_particle_system_z, glm::vec3(0.0f, 0.0f, 1.0f));
            particles_position[i] += tmp;
            model = glm::translate(model, particles_position[i]);
            model = glm::scale(model, glm::vec3(0.005f));
            ourShader.setMat4("model", model);
            ourShader.setFloat("alpha", 1.0f);

            renderSphere(16, 16);
        }

        ourShader.setVec3("material.specular", glm::vec3(0.6f, 0.7f, 0.6f));
        ourShader.setFloat("material.shininess", 84.0f);

        ourShader.setVec3("material.ambient", glm::vec3(0.02f, 0.2f, 0.02f));
        ourShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.6f, 0.07f));

        model = glm::mat4(1.0f);
        model = glm::rotate(model, rotation_angle_particle_system_y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation_angle_particle_system_x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation_angle_particle_system_z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        ourShader.setMat4("model", model);
        ourShader.setFloat("alpha", 1.0f);

        glBindVertexArray(cubeVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, rotation_angle_particle_system_y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation_angle_particle_system_x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation_angle_particle_system_z, glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourShader.setFloat("alpha", 0.5f);

        glBindVertexArray(cubeVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //LAMP RENDERING
        lamp_shader.use();
        lamp_shader.setMat4("projection", projection);
        lamp_shader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::rotate(model, rotation_angle_lamp_y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation_angle_lamp_x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, -rotation_angle_lamp_z, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        lamp_shader.setMat4("model", model);

        lamp_model.Draw(lamp_shader);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glfwTerminate();
    return 0;
}

void update_lamp_pos(float rotation_angle, glm::vec3 rotation_axis)
{
    glm::mat4 transform(1.0f);
    transform = glm::rotate (transform, rotation_angle, rotation_axis);

    glm::vec4 lightPos_tmp = glm::vec4(lightPos, 1.0f) * transform;
    lightPos = glm::vec3(lightPos_tmp.x, lightPos_tmp.y, lightPos_tmp.z);
}

void processInput(GLFWwindow* window)
{
    //Input handling for window exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Inputs for handling the particles position initialization
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE && init_position_press)
        init_position_press = false;

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !init_position_press)
    {
        init_position_press = true;
        init_position = !init_position;
        
        if (!init_position)
            initialized = false;
    }

    //Inputs for handling the light movement (Forward, Backward)

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        lightPos += -lightPos * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        lightPos -= -lightPos * deltaTime;

    //Inputs for handling the spotlight model rotation
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        rotation_angle_lamp_y -= deltaTime;
        update_lamp_pos(deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        rotation_angle_lamp_y += deltaTime;
        update_lamp_pos(-deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        rotation_angle_lamp_x += deltaTime;
        update_lamp_pos(-deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        rotation_angle_lamp_x -= deltaTime;
        update_lamp_pos(deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        rotation_angle_lamp_z += deltaTime;
        update_lamp_pos(-deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        rotation_angle_lamp_z -= deltaTime;
        update_lamp_pos(deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    //Inputs for handling the cube rotations on x, y and z axis
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotation_angle_particle_system_y -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotation_angle_particle_system_y += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotation_angle_particle_system_x -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotation_angle_particle_system_x += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        rotation_angle_particle_system_z -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        rotation_angle_particle_system_z += deltaTime;

    //Inputs for handling the camera position on z axis
    float velocity = 1.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera_position -= glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera_position += glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void init_particles_position()
{
    if (!initialized)
    {
        for (int i = 0; i < particles_position.size(); i++)
        {
            glm::vec3 tmp = glm::vec3(0.1f);
            for (int j = 0; j < 3; j++)
            {
                float a = 0.19;
                float random = ((float)rand()) / (float)RAND_MAX;
                float r = random * a;

                float negate = (std::rand() % 2 == 0) ? -1 : 1;

                tmp[j] = r * negate;
            }

            particles_position[i] = tmp;
            initialized = true;
        }
    }
}

glm::vec3 move_particle(int particle_number, float unit)
{
    glm::vec3 tmp = glm::vec3(0.0f);

    for (int j = 0; j < 3; j++)
    {
        float negate = (std::rand() % 2 == 0) ? -1 : 1;

        unit = 0.01 * negate;

        glm::vec3 next;
        next[j] = particles_position[particle_number][j] + unit;

        tmp[j] = (next[j] < -0.4f || next[j] > 0.4f) ? 0.0f : unit;
    }

    return tmp;
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere(unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS)
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);

            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}