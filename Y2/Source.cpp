#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const int w_width = 800;
const int w_height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void input(GLFWwindow* window, double dt);
void debugShader(unsigned int& vertexShader);// check compile vertex and fragment shader
void debugProgram(unsigned int& shaderProgram); // check for errors when linking shader program
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

// cam properties
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

float dt = 0.0f; // time between current frame and last frame 
float lf = 0.0f; // time of last frame

float lastX = 400; // init the last mouse positions on the center of the skin 
float lastY = 300;

float fov = 45.0f;

float yaw, pitch = 0.0f;

bool first = true;

glm::vec3 lightPos(0.0f, 0.0f,  2.0f);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Y2 Framework", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window " << std::endl;
        glfwTerminate();
        return -1; // error number
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    Shader shader("vertex.shader", "fragment.shader"); // vertex then fragment
    Shader lighting_shader("lightingVertex.shader", "lightingFragment.shader");
    Shader lightSource("lightSourceVertex.shader", "lightSourceFragment.shader");

   float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

    //unsigned VBO, VAO;
    //glGenBuffers(1, &VBO);
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO); // bind vao

    //glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vbo 
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // fill vbo with data
    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    ////// texture attribute
    ////glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    ////glEnableVertexAttribArray(1);

  // first, configure the cube's VAO (and VBO)
   unsigned int VBO, cubeVAO;
   glGenVertexArrays(1, &cubeVAO);
   glGenBuffers(1, &VBO);

   glBindVertexArray(cubeVAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   // position attribute
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
   unsigned int lightCubeVAO;
   glGenVertexArrays(1, &lightCubeVAO);
   glBindVertexArray(lightCubeVAO);

   // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

    // Texture chapter
    unsigned int texture1;
    unsigned int texture2;
    // first texture
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set behaviour of texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // first is texture target, second is the axis and third is the texture wrapping option  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("images/troll.jpg", &width, &height, &nrChannels, 0); // load image 1
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load texture" << std::endl;
    }

    stbi_image_free(data); // free image memory, good practice

    // second texture
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set behaviour of texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // first is texture target, second is the axis and third is the texture wrapping option  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    data= stbi_load("images/awesomeface.png", &width, &height, &nrChannels, 0); // load image 2
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // last param is data of image 
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load texture" << std::endl;
    }

    stbi_image_free(data); // free image memory, good practice

    shader.employ();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    glm::vec3 cubePos[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    glm::vec3 objectColor = { 1.0f, 0.5f, 0.31f };
    glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
    // ------------------------------------------------
    while (!glfwWindowShouldClose(window)) { // Game loop
        float current = glfwGetTime(); // delta time, get current time
        dt = current - lf; // time it took for each frame
        lf = current; 

        // input
        input(window, dt);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        glEnable(GL_DEPTH_TEST); // depth is stored as the fragment as its z's value, it is compared with z buffer

        // rendering
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
    
       //shader.employ(); // use shader program
       //glBindVertexArray(VAO); // always bind the buffer before rendering to specify the correct settings

        lighting_shader.employ();
        lighting_shader.setVec3("lightColor", lightColor); // set color for the light
        lighting_shader.setVec3("objectColor", objectColor); // set color for the object 

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)w_width / (float)w_height, 0.1f, 100.f); // projection matrix
        lighting_shader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(camPos, camPos + camDir, camUp); // view matrix
        lighting_shader.setMat4("view", view);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //for (int i = 0; i < 10; ++i) {
        //    model = glm::mat4(1.0f); // every iteration create model matrix because each cube have different position
        //    model = glm::translate(model, cubePos[i]); // model matrix
        //    float angle = 20.0f * i; // angle increases over each iteration
        //    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // last param is which axis to rotate by
        //    shader.setMat4("model", model);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}
        //
        lightSource.employ();
        lightSource.setMat4("projection", projection); // dont have to projection and view matrix
        lightSource.setMat4("view", view);
        model = glm::mat4(1.0f); // want to translate so reuse the model matrix
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2));
        lightSource.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // check and call events (check if user close the window)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // deallocate
  //glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, 800, 600);
}

void input(GLFWwindow* window, double dt)
{
    const float speed = 3.0 * dt; // speed of cam
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    // wireframe input
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    // camera movement input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += speed * camDir;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= speed * camDir;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= speed * glm::normalize(glm::cross(camDir, camUp));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += speed * glm::normalize(glm::cross(camDir, camUp));
}

void debugShader(unsigned int &vertexShader)
{
    int success;
    char log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
    }
}

void debugProgram(unsigned int& shaderProgram)
{
    int success;
    char log[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
    }
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (first) 
    {
        lastX = xPos;
        lastY = yPos;
        first = false;
    }
    float offsetX = xPos - lastX; // the result here is how much the cam's x axis has moved
    float offsetY = lastY - yPos;
    lastX = xPos; // update the lastX and lastY
    lastY = yPos;

    const float sens = 0.1f;
    offsetX *= sens;
    offsetY *= sens;

    yaw += offsetX; // how much we look around
    pitch += offsetY; // how much we look up

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction; // exact direction of where camera is looking at after calculating mouse movement
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camDir = glm::normalize(direction); // length of direction vector always one 
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
