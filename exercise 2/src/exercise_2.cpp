/*  GLAD is an open source library, it has a slightly different configuration setup than most common open source libraries. 
 *  GLAD uses a web service where we can tell GLAD for which version of OpenGL we'd like to define and load all relevant,
 *   OpenGL functions according to that version.
    
 *   --IMPORTANT--
 *   Be sure to include GLAD before GLFW. 
 *   The include file for GLAD includes the required OpenGL headers behind the scenes (like GL/gl.h) so be sure to include GLAD 
 *   before other header files that require OpenGL (like GLFW).
*/
#include <glad/glad.h>

/*  GLFW is a library, written in C, specifically targeted at OpenGL.
 *  GLFW gives us the bare necessities required for rendering goodies to the screen.
 *  It allows us to create an OpenGL context, define window parameters, and handle user input.
 */ 
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
    //  We first initialize GLFW in order to configure it.
    glfwInit();

    /*  We configure GLFW using glfwWindowHint. This method uses three arguments:
     *      1. the first argument specifies what option to configure, here we can select the option from a large enum of possible options prefixed with GLFW_. 
     *      2. the second argument is an integer that sets the value of our option. Since the OpenGL version that we use is the 3.3 we set:
     *          a. the minor version to 3; 
     *          b. the major version to 3;
     *          c. we also tell GLFW we want to explicitly use the core-profile, in this way we restrict the features to a smaller set (only those that we need).
    */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /*  The glfwCreateWindow function requires:
     *      1. the window width;
     *      2. the window height;
     *      3. the name for the window. 
     *  We can ignore the last 2 parameters. 
     *  The function returns a GLFWwindow object that we'll later need for other GLFW operations.
    */

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // We tell GLFW to make the context of our window the main context on the current thread.
    glfwMakeContextCurrent(window);

    /*  We tell GLFW we want to call this function on every window resize by registering it. 
     *  When the window is first displayed framebuffer_size_callback gets called as well with the resulting window dimensions. 
     *  For retina displays width and height will end up significantly higher than the original input values.
    */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /*  We pass GLAD the function to load the address of the OpenGL function pointers which is OS - specific.
     *  GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for.
    */
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

    /*  In order to start drawing something we have to first give OpenGL some input vertex data. 
     *  OpenGL is a 3D graphics library so all coordinates that we specify in OpenGL are in 3D(x, y and z coordinate).
     *  OpenGL doesn't simply transform all your 3D coordinates to 2D pixels on your screen; 
     *  OpenGL only processes 3D coordinates when they're in a specific range between - 1.0 and 1.0 on all 3 axes(x, y and z).
     *  All coordinates within this so called normalized device coordinates range will end up visible on your screen
        (and all coordinates outside this region won't).
     *  -------------OBSERVATIONS------------- 
     *  In order to render something we want to specify some vertices, each one having a 3D position.
     *  We define them in normalized device coordinates(the visible region of OpenGL) in a float array:
    */


    /* In this case we define all the vertex 3D positions needed to draw each tile of the chessboard.
     * Since each tile is composed from two triangle 6 vertex 3D positions have been defined. 
     */
    float tile_vertices[] = {
        // positions        //normals
        -0.9f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f,
        -0.9f, 0.0f, -0.9f, 0.0f, 1.0f, 0.0f,
    };

    /* In this case we define all the vertex 3D positions needed to draw a cube within the scene.
     * Since the cube is composed from 6 side and each side is composed from two triangle 36 vertex 3D positions have been defined.
     */
    float cube_vertices[] = {
        // positions            //normals
        -0.1f, -0.1f, -0.1f,    0.0f,  0.0f, -1.0f,
         0.1f, -0.1f, -0.1f,    0.0f,  0.0f, -1.0f,
         0.1f,  0.1f, -0.1f,    0.0f,  0.0f, -1.0f,
         0.1f,  0.1f, -0.1f,    0.0f,  0.0f, -1.0f,
        -0.1f,  0.1f, -0.1f,    0.0f,  0.0f, -1.0f,
        -0.1f, -0.1f, -0.1f,    0.0f,  0.0f, -1.0f,

        -0.1f, -0.1f,  0.1f,    0.0f,  0.0f,  1.0f,
         0.1f, -0.1f,  0.1f,    0.0f,  0.0f,  1.0f,
         0.1f,  0.1f,  0.1f,    0.0f,  0.0f,  1.0f,
         0.1f,  0.1f,  0.1f,    0.0f,  0.0f,  1.0f,
        -0.1f,  0.1f,  0.1f,    0.0f,  0.0f,  1.0f,
        -0.1f, -0.1f,  0.1f,    0.0f,  0.0f,  1.0f,

        -0.1f,  0.1f,  0.1f,   -1.0f,  0.0f,  0.0f,
        -0.1f,  0.1f, -0.1f,   -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f, -0.1f,   -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f, -0.1f,   -1.0f,  0.0f,  0.0f,
        -0.1f, -0.1f,  0.1f,   -1.0f,  0.0f,  0.0f,
        -0.1f,  0.1f,  0.1f,   -1.0f,  0.0f,  0.0f,

         0.1f,  0.1f,  0.1f,    1.0f,  0.0f,  0.0f,
         0.1f,  0.1f, -0.1f,    1.0f,  0.0f,  0.0f,
         0.1f, -0.1f, -0.1f,    1.0f,  0.0f,  0.0f,
         0.1f, -0.1f, -0.1f,    1.0f,  0.0f,  0.0f,
         0.1f, -0.1f,  0.1f,    1.0f,  0.0f,  0.0f,
         0.1f,  0.1f,  0.1f,    1.0f,  0.0f,  0.0f,

        -0.1f, -0.1f, -0.1f,    0.0f, -1.0f,  0.0f,
         0.1f, -0.1f, -0.1f,    0.0f, -1.0f,  0.0f,
         0.1f, -0.1f,  0.1f,    0.0f, -1.0f,  0.0f,
         0.1f, -0.1f,  0.1f,    0.0f, -1.0f,  0.0f,
        -0.1f, -0.1f,  0.1f,    0.0f, -1.0f,  0.0f,
        -0.1f, -0.1f, -0.1f,    0.0f, -1.0f,  0.0f,

        -0.1f,  0.1f, -0.1f,    0.0f,  1.0f,  0.0f,
         0.1f,  0.1f, -0.1f,    0.0f,  1.0f,  0.0f,
         0.1f,  0.1f,  0.1f,    0.0f,  1.0f,  0.0f,
         0.1f,  0.1f,  0.1f,    0.0f,  1.0f,  0.0f,
        -0.1f,  0.1f,  0.1f,    0.0f,  1.0f,  0.0f,
        -0.1f,  0.1f, -0.1f,    0.0f,  1.0f,  0.0f
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

    /*  With the vertex data defined for the tiles and the cube we'd like to send it as input to the first process 
     *  of the graphics pipeline: the vertex shader. 
     *  This is done by creating memory on the GPU where we store the vertex data, configure how OpenGL should interpret 
     *  the memory and specify how to send the data to the graphics card. 
     *  The vertex shader then processes as much vertices as we tell it to from its memory.
     *  We manage this memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory. 
     *  The advantage of using those buffer objects is that we can send large batches of data all at once to the graphics card, 
     *  and keep it there if there's enough memory left, without having to send data one vertex at a time. 
     *  Sending data to the graphics card from the CPU is relatively slow, so wherever we can we try to send as much data as possible at once. 
     *  Once the data is in the graphics card's memory the vertex shader has almost instant access to the vertices making it extremely fast.
     *  A vertex buffer object is our first occurrence of an OpenGL object. 
     *  Just like any object in OpenGL, this buffer has a unique ID corresponding to that buffer, 
     *  so we can generate one with a buffer ID using the glGenBuffers function.
     */
    unsigned int tileVBO, tileVAO;
    glGenBuffers(1, &tileVBO);

    /*  A vertex array object (also known as VAO) can be bound just like a vertex buffer object and any subsequent vertex attribute 
     *  calls from that point on will be stored inside the VAO. 
     *  This has the advantage that when configuring vertex attribute pointers you only have to make those calls once and whenever we want to draw the object, 
     *  we can just bind the corresponding VAO. 
     *  This makes switching between different vertex data and attribute configurations as easy as binding a different VAO. 
     *  All the state we just set is stored inside the VAO.
     *  A vertex array object stores the following:
     *  1.  Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
     *  2.  Vertex attribute configurations via glVertexAttribPointer.
     *  3.  Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
    */
    glGenVertexArrays(1, &tileVAO);

    /*  To use a VAO we have to bind the VAO using glBindVertexArray. 
     *  From that point on we should bind/configure the corresponding VBO(s) and attribute pointer(s) 
     *  and then unbind the VAO for later use. 
     *  As soon as we want to draw an object, we simply bind the VAO with the preferred settings before drawing the object and that is it. 
    */
    glBindVertexArray(tileVAO);

    /*  OpenGL has many types of buffer objectsand the buffer type of a vertex buffer object is GL_ARRAY_BUFFER.
     *  OpenGL allows us to bind to several buffers at once as long as they have a different buffer type.
     *  We can bind the newly created buffer to the GL_ARRAY_BUFFER target with the glBindBuffer function.
    */
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);

    /*  The glBufferData function copies the previously defined vertex data into the buffer's memory. 
     *  glBufferData is a function specifically targeted to copy user-defined data into the currently bound buffer. 
     *      1.  Its first argument is the type of the buffer we want to copy data into: 
     *          the vertex buffer object currently bound to the GL_ARRAY_BUFFER target. 
     *      2.  The second argument specifies the size of the data (in bytes) we want to pass to the buffer; 
     *          a simple sizeof of the vertex data suffices. 
     *      3.  The third parameter is the actual data we want to send.
     *      4.  The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms:
     *          a.  GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
     *          b.  GL_STATIC_DRAW: the data is set only once and used many times.
     *          c.  GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
     *   The position data of each triangle does not change, is used a lot, and stays the same for every render call so its 
     *   usage type should best be GL_STATIC_DRAW. If, for instance, one would have a buffer with data that is likely to change frequently, 
     *   a usage type of GL_DYNAMIC_DRAW ensures the graphics card will place the data in memory that allows for faster writes.
     *   As of now we stored the vertex data within memory on the graphics card as managed by a vertex buffer object named VBO. 
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);

    // position attribute
    /*  The function glVertexAttribPointer has quite a few parameters so let's carefully walk through them:
     *      1.  The first parameter specifies which vertex attribute we want to configure. 
     *          This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
     *      2.  The next argument specifies the size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
     *      3.  The third argument specifies the type of the data which is GL_FLOAT (a vec* in GLSL consists of floating point values).
     *      4.  The next argument specifies if we want the data to be normalized. If we're inputting integer data types (int, byte) 
     *          and we've set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float. 
     *          This is not relevant for us so we'll leave this at GL_FALSE.
     *      5.  The fifth argument is known as the stride and tells us the space between consecutive vertex attributes. 
     *          Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride. 
     *          Note that since we know that the array is tightly packed (there is no space between the next vertex attribute value) 
     *          we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). 
     *          Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we'll get to 
     *          see more examples of that later on.
     *      6.  The last parameter is of type void* and thus requires that weird cast. 
     *          This is the offset of where the position data begins in the buffer. 
     *          Since the position data is at the start of the data array this value is just 0.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    /*  We don't want the application to draw a single image and then immediately quit and close the window. 
     *  We want the application to keep drawing images and handling user input until the program has been explicitly told to stop. 
     *  For this reason we have to create a while loop, that we now call the render loop (frame), that keeps on running until we tell GLFW to stop. 
     *
     *  The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close. 
     *  If so, the function returns true and the render loop stops running, after which we can close the application.
    */

    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // We call the method processInput defined within each render loop.
        processInput(window, object_position_size);

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /*  We also specify the color to clear the screen with using glClearColor.
         *  Whenever we call glClear and clear the color buffer, the entire color buffer will be filled with the color as configured by glClearColor.
        */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        /*  glClear is used to clear the screen color buffet, where we pass in buffer bits to specify which buffer we would like to clear.
         *  The possible bits we can set are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT.
        */
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

                /*  The glDrawArrays function takes as its first argument the OpenGL primitive type we would like to draw.
                 *      1.  Since we wanted to draw a triangle, we pass in GL_TRIANGLES. 
                 *      2.  The second argument specifies the starting index of the vertex array we'd like to draw; we just leave this at 0. 
                 *      3.  The last argument specifies how many vertices we want to draw, which is 3 (we only render 1 triangle from our data, which is exactly 3 vertices long).
                */
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
        
        /*  The glfwSwapBuffers will swap the color buffer(a large 2D buffer that contains color values for each pixel in GLFW's window),
         *   that is used to render to during this render iteration and show it as output to the screen.
        */
        glfwSwapBuffers(window);

        /*  The glfwPollEvents function:
         *  1.  checks if any events are triggered(like keyboard input or mouse movement events);
         *  2.  updates the window state;
         *  3.  and calls the corresponding functions(which we can register via callback methods).
         *
        */
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &tileVAO);
    glDeleteBuffers(1, &tileVBO);

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    
    /*  As soon as we exit the render loop we would like to properly clean/delete all of GLFW's resources that were allocated. 
     *  We can do this via the glfwTerminate function that we call at the end of the main function.
    */
    glfwTerminate();
    return 0;
}

/*  We also want to have some form of input control in GLFW and we can achieve this with several of GLFW's input functions. 
 *  We'll be using GLFW's glfwGetKey function that takes the window as input together with a key. 
 *  The function returns whether this key is currently being pressed. 
 *  We're creating a processInput function to keep all input code organized.
 *  Here we check:
 *  1. whether the user has pressed the escape key (if it's not pressed, glfwGetKey returns GLFW_RELEASE). 
 *     If the user did press the escape key, we close GLFW by setting its WindowShouldClose property to true using glfwSetwindowShouldClose. 
 *     The next condition check of the main while loop will then fail and the application closes.
 *  2. whether the user has pressed a movement key (W,A,S and D). In this case we call the method ProcessKeyboard defined in the Camera class.
 *  3. wether the user has pressed the light switching key (L). In this case we update the value of the ligth_changer boolean variable.   
*/
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

/*  The moment a user resizes the window the viewport should be adjusted as well. 
 *  We register a callback function on the window that gets called each time the window is resized. 
*/ 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    /*  Through the use of glViewport method we tell OpenGL the size of the rendering window, 
     *  so OpenGL knows how we want to display the data and coordinates with respect to the window. 
     *  The glViewport method takes some parameters:
     *      1. the first two parameters of glViewport set the location of the lower left corner of the window;
     *      2. the third and fourth parameter set the width and height of the rendering window in pixels, which we set equal to GLFW's window size.
     * 
     *  -------------OBSERVATIONS-------------
     *  Behind the scenes OpenGL uses the data specified via glViewport to transform the 2D coordinates it processed to coordinates on your screen. 
     *  For example, a processed point of location (-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coordinates. 
     *  Note that processed coordinates in OpenGL are between -1 and 1 so we effectively map from the range (-1 to 1) to (0, 800) and (0, 600).
    */
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