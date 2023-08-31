#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Eigen/Dense>
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using Eigen::Matrix4f;
using Eigen::Vector4f;
using Eigen::Vector3f;
# define PI  3.14159265358979323846f

//Key Press Input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}




GLFWwindow* InitWindow(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //create a window object. This window object holds all the windowing data and is required by most of GLFW's other functions.
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    //we want to initialize GLAD before we call any OpenGL function:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}
unsigned int GenerateTexture(const char* fileName, const bool alpha = false, const bool flipY = false)
{
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    int width, height, nrChannels;
    if(flipY)
        stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, alpha?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture1;
}


int main() {

    auto window = InitWindow(1200,800);
    if (!window) {
        return -1;
    }

    // Create the shader program
    Shader program_orange("vertex.vert", "orange.frag");
    Shader program_blue("vertex.vert", "blue.frag");
    Shader program_txtr("vertex.vert", "txtr.frag");


#pragma region Triangle with vertex color
    //Generate a Vertex Array Object
    unsigned int VAO_triangle;
    glGenVertexArrays(1, &VAO_triangle);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO_triangle);
    //generate a VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // A simple vertex array
    float vertices_triangle[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_triangle), vertices_triangle, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#pragma endregion

#pragma region Rect changing color
    //Using elements and indices 
    unsigned int VAO_rect;
    glGenVertexArrays(1, &VAO_rect);
    glBindVertexArray(VAO_rect);
    unsigned int VBO_rect;
    glGenBuffers(1, &VBO_rect);
    float vertices_rect[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_rect), vertices_rect, GL_STATIC_DRAW);
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    //Get an element buffer instead
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
#pragma endregion


#pragma region Textures
    //Get a texture please
    unsigned int vao_txtr;
    glGenVertexArrays(1, &vao_txtr);
    glBindVertexArray(vao_txtr);

    auto texture1 = GenerateTexture("uv.jpg");
    auto texture2 = GenerateTexture("face.png",true,true);

    float vertices_txtr[] = {
	    // positions          // colors           // texture coords
	    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int vbo_txtr;
    glGenBuffers(1, &vbo_txtr);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_txtr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_txtr), vertices_txtr, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int ebo_txtr;
    glGenBuffers(1, &ebo_txtr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_txtr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    program_txtr.use();
    program_txtr.setInt("texture1", 0);
    program_txtr.setInt("texture2", 1);
#pragma endregion
    Matrix4f mat_trans = Matrix4f::Identity();
    Eigen::Quaternion<float> quat;
	quat = Eigen::AngleAxis<float>(PI*0.25, Vector3f(0,0,1));
    mat_trans.block<3, 3>(0, 0) = quat.normalized().toRotationMatrix();
    std::cout << mat_trans << std::endl;
    program_txtr.setMat4f("transform", mat_trans);


    //The main render loop
    while (!glfwWindowShouldClose(window))
    {
        //Input
        processInput(window);

        //clearing color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);




         //4. draw the object
//#pragma region Draw Rect with changing color
//        program_blue.use();
//        float timeValue = glfwGetTime();
//        float blueValue = sin(timeValue) / 2.0f + 0.5f;
//        program_blue.setFloat4("ourColor", 0.f, 0.f, blueValue, 1.0f);
//        glBindVertexArray(VAO_rect);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//#pragma endregion
//
//#pragma region Draw Triangle with Vertex Color
//        program_orange.use();
//        glBindVertexArray(VAO_triangle);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//#pragma endregion

        program_txtr.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(vao_txtr);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}