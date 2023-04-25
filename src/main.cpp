#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "../libs/gl/glad.h"
#include "../libs/gl/glfw3.h"
#include "../libs/tinygltf/tinygltf.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glWrapper/glWrapper.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Not working!\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "Test", NULL, NULL);
    if (!window)
    {
        std::cout << "No window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load GLAD\n";
        glfwTerminate();
        return -1;
    }

    // Create shader object
    glWrap::Shader shader("assets/vertex.glsl", "assets/fragment.glsl");

    std::vector<std::unique_ptr<glWrap::Mesh>> meshes;

    if (glWrap::loadModel("assets/Triangle.gltf", meshes)){
        std::cout << "Issue!";
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();

        /*
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
        */

        for (int i{}; i < meshes.size(); ++i){
            meshes[i].get()->Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}