#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"

const int w_width = 800;
const int w_height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void input(GLFWwindow* window);
void debugShader(unsigned int& vertexShader);// check compile vertex and fragment shader
void debugProgram(unsigned int& shaderProgram); // check for errors when linking shader program

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    Shader shader("vertex.shader", "fragment.shader"); // vertex then fragment


    // Texture chapter
    float texCoords[] = { // texture 
        0.0f, 0.0f, // lower left corner
        1.0f, 0.0f, // lower right corner
        0.5f, 1.0f, // top center corner
    };
    unsigned int texture;
    unsigned int texture2;
    // first texture
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set behaviour of texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // first is texture target, second is the axis and third is the texture wrapping option  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels; 
    unsigned char* data = stbi_load("images/troll.jpg", &width, &height, &nrChannels, 0); // load image 1
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load texture" << std::endl;
    }

    // second texture
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set behaviour of texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // first is texture target, second is the axis and third is the texture wrapping option  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char* data2 = stbi_load("images/awesomeface.png", &width, &height, &nrChannels, 0); // load image 2
     if (data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2); // last param is data of image 
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load texture" << std::endl;
    }

    stbi_image_free(data); // free image memory, good practice

    shader.employ();
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    shader.setInt("texture2", 1);
    stbi_set_flip_vertically_on_load(true);

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = { // for ebo to store indices
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    //// left triangle
    //float first[] = {
    //      -0.9f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,// left 
    //      -0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f  // right
    //      -0.45f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top
    //};
    //// right triangle
    //float second[] = {
    //     0.0f, -0.5f, 0.0f,
    //     0.9f, -0.5f, 0.0f,
    //     0.5f, 0.5f, 0.0f,
    //};
    
    unsigned int EBO;
    unsigned int VBO[2];
    unsigned int VAO[2];
    glGenBuffers(1, &EBO);
    glGenBuffers(2, VBO);
    glGenVertexArrays(2, VAO);
    // Bind vertex array (first triangle)
    glBindVertexArray(VAO[0]);
    // Copy vertices into vbo
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // bind buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // link vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // when the first of the color attribute starts, 3 positions 
    glEnableVertexAttribArray(1); //  need to enable this cos if never use it will not be used for rendering
    // texture attributes
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // 6 positions after first is first position of the texture attribute
    glEnableVertexAttribArray(2);


    // ------------------------------------------------
    while (!glfwWindowShouldClose(window)) { // Game loop
        // input
        input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // rendering
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // glUseProgram(shaderProgram); 
        shader.employ(); // use shader program
        glBindVertexArray(VAO[0]); // always bind the buffer before rendering to specify the correct settings
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /*float timeValue = glfwGetTime();            
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

        // draw first triangle using the data from the first VAO
        {
            // glUseProgram(shaderProgram);
            // glBindVertexArray(VAO[1]); // unbind vao
            // draw first triangle using the data from the second VAO
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            //glBindVertexArray(0); // unbind it
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INTs, 0);
        }
        // check and call events (check if user close the window)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, 800, 600);
}

void input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
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
