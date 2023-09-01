#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Eigen/Dense>
#include "shader.h"
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using Eigen::Matrix4f;
using Eigen::Vector4f;
using Eigen::Vector3f;
# define PI  3.14159265358979323846f

//Some global variables we will access
Vector3f cameraPos(0, 0, 3.0f);
Vector3f cameraFront(0, 0, -1.0f);
Vector3f cameraUp(0, 1.0, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


//Key Press Input
void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraFront.cross(cameraUp).normalized()* cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraFront.cross(cameraUp).normalized()* cameraSpeed;
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
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    //we want to initialize GLAD before we call any OpenGL function:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
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
	constexpr int width = 1200;
	constexpr int height = 800;
	const auto window = InitWindow(1200,800);
    if (!window) {
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    // Create the shader program
    Shader program_orange("vertex.vert", "orange.frag");
    Shader program_blue("vertex.vert", "blue.frag");
    Shader program_txtr("vertex.vert", "txtr.frag");


#pragma region Triangle with vertex color
    //Generate a Vertex Array Object
    unsigned int vao_triangle;
    glGenVertexArrays(1, &vao_triangle);
    // 1. bind Vertex Array Object
    glBindVertexArray(vao_triangle);
    //generate a VBO
    unsigned int vbo_vertColor;
    glGenBuffers(1, &vbo_vertColor);

    // A simple vertex array
	constexpr float vertices_triangle[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertColor);
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
    unsigned int vao_rect;
    glGenVertexArrays(1, &vao_rect);
    glBindVertexArray(vao_rect);
    unsigned int vbo_rect;
    glGenBuffers(1, &vbo_rect);
	constexpr float vertices_rect[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_rect), vertices_rect, GL_STATIC_DRAW);
	const unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    //Get an element buffer instead
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
#pragma endregion


#pragma region Textures
    //Get a texture please
    unsigned int vao_txtr;
    glGenVertexArrays(1, &vao_txtr);
    glBindVertexArray(vao_txtr);

	const auto texture1 = GenerateTexture("uv.jpg");
	const auto texture2 = GenerateTexture("face.png",true,true);

	constexpr float vertices_txtr[] = {
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



    //The main render loop
    while (!glfwWindowShouldClose(window))
    {
        //Calculate Time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        //Input
        processInput(window);

        //clearing color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //clear depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





         //4. draw the object
//#pragma region Draw Rect with changing color
//        program_blue.use();
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


#pragma region Transformation matrices
        Matrix4f mat_trans = Matrix4f::Identity();
        Eigen::Quaternion<float> quat;
        quat = Eigen::AngleAxis<float>(currentFrame *PI * 0.25, Vector3f(0, 0, 1));
        mat_trans.block<3, 3>(0, 0) = quat.normalized().toRotationMatrix();
        program_txtr.setMat4f("model", mat_trans);

        // Not the actual Direction, reversed
     


        auto mat_view = GetLookAtMat(cameraPos, cameraPos + cameraFront, cameraUp);
    	program_txtr.setMat4f("view", mat_view);
        auto mat_pers = GetMatPerspectiveProjection(PI * 0.25, (float)width / (float)height, 0.1, 100.0);
        program_txtr.setMat4f("projection", mat_pers);

#pragma endregion


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