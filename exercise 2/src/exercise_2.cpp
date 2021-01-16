#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, glm::vec4 object_position[]);
void renderSphere(unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int NUMBER_CHESSBOARD_TILES = 25;
const unsigned int OBJECTS_NUMBER = 4;

// camera
Camera camera(glm::vec3(0.0f, 0.25f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool light_changer = false;
bool switcher_press = false;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.40f, 2.0, 0.40f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("light_casters.vs", "light_casters.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float tile_vertices[] = {
        // positions        
        -0.9f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
    };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cube_vertices[] = {
        -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,
         0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,
         0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,
         0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,
        -0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,
        -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,

        -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,
         0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,
         0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,
        -0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,
        -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,

        -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,
        -0.1f,  0.1f, -0.1f, -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f,  0.1f, -1.0f,  0.0f,  0.0f,
        -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,

         0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,
         0.1f,  0.1f, -0.1f,  1.0f,  0.0f,  0.0f,
         0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,
         0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,
         0.1f, -0.1f,  0.1f,  1.0f,  0.0f,  0.0f,
         0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,

        -0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,
         0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,
         0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,
         0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,
        -0.1f, -0.1f,  0.1f,  0.0f, -1.0f,  0.0f,
        -0.1f, -0.1f, -0.1f,  0.0f, -1.0f,  0.0f,

        -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,
         0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,
        -0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,
        -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f
    };

    glm::vec4 object_position_size[] = {
        glm::vec4(0.2f, 0.0f, 0.6f, 0.15),
        glm::vec4(0.5f, 0.5f, 0.5f, 0.15),
        glm::vec4(0.7f, 0.0f, 0.2f, 0.15),
        glm::vec4(0.6f, 0.07f, 0.8f, 0.15),
    };

    glm::vec3 tile_ambient_diffuse[2][2] =
    {
        //ambient                       //diffuse
        {glm::vec3(0.0f, 0.0f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, 0.5f),  glm::vec3(1.0f, 1.0f, 1.0f)},
    };

    glm::vec3 object_ambient_diffuse[4][2] =
    {
        //ambient                       //diffuse
        {glm::vec3(0.0f, 0.18f, 0.0f),  glm::vec3(0.07568f, 0.61f, 0.07568f)},
        {glm::vec3(0.0f, 0.0f, 0.18f),  glm::vec3(0.07568f, 0.07568f, 0.61f)},
        {glm::vec3(0.18f, 0.0f, 0.0f),  glm::vec3(0.61f, 0.07568f, 0.07568f)},
    };

    unsigned int tileVBO, tileVAO;
    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &tileVBO);

    glBindVertexArray(tileVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // input
        // -----
        processInput(window, object_position_size);

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // get matrix's uniform location and set matrix
        // be sure to activate shader when setting uniforms/drawing objects
        ourShader.use();
        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("light.direction", glm::vec3(0.0f, -1.0f, 0.0f));
        ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("viewPos", camera.Position);

        // light properties
        ourShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("light.constant", 1.0f);
        ourShader.setFloat("light.linear", 0.09f);
        ourShader.setFloat("light.quadratic", 0.032f);

        ourShader.setBool("spotlight", light_changer);

        std::cout << light_changer << std::endl;

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.setMat4("model", model);
        
        glBindVertexArray(tileVAO);

        ourShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setFloat("material.shininess", 128.0f);

        bool color_switcher = true;
        for (int i = 0; i < NUMBER_CHESSBOARD_TILES; i++)
            for (int j = 0; j < NUMBER_CHESSBOARD_TILES; j++)
            {
                ourShader.setVec3("material.ambient", tile_ambient_diffuse[color_switcher][0]);
                ourShader.setVec3("material.diffuse", tile_ambient_diffuse[color_switcher][1]);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i * 0.1f, 0.0f, j * 0.1f));
                ourShader.setMat4("model", model);

                color_switcher = !color_switcher;

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

        ourShader.setVec3("material.specular", glm::vec3(0.94f, 0.94f, 0.94f));
        ourShader.setFloat("material.shininess", 111.0f);

        for (int i = 0; i < 3; ++i)
        {
            ourShader.setVec3("material.ambient", object_ambient_diffuse[i][0]);
            ourShader.setVec3("material.diffuse", object_ambient_diffuse[i][1]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(object_position_size[i].x, object_position_size[i].y, object_position_size[i].z));
            model = glm::scale(model, glm::vec3(0.11f));
            ourShader.setMat4("model", model);

            renderSphere(32, 32);
        }
        
        ourShader.setVec3("material.ambient", glm::vec3(0.67f, 0.0f, 0.0f));
        ourShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.67f, 0.41f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(object_position_size[3].x, object_position_size[3].y, object_position_size[3].z));
        model = glm::scale(model, glm::vec3(0.7f));
        ourShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &tileVAO);
    glDeleteBuffers(1, &tileVBO);

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, glm::vec4 object_position[])
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && switcher_press)
        switcher_press = false;

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !switcher_press)
    {
        switcher_press = true;
        light_changer = !light_changer;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, object_position, OBJECTS_NUMBER);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, object_position, OBJECTS_NUMBER);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, object_position, OBJECTS_NUMBER);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, object_position, OBJECTS_NUMBER);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
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