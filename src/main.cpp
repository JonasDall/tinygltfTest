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

    /*
    // Start loading file
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string error{};
    std::string warning{};

    if (!loader.LoadASCIIFromFile(&model, &error, &warning, "assets/Triangle.gltf"))
    {
        std::cout << error << " | " << warning << '\n';
        return 1;
    }

    tinygltf::Mesh mesh = model.meshes[0];
    tinygltf::Primitive primitive = mesh.primitives[0];

    int accessorLoc = primitive.attributes.at("POSITION");
    std::cout << accessorLoc << " Access loc\n";

    tinygltf::Accessor accessor = model.accessors[accessorLoc];
    int bufferviewLoc = accessor.bufferView;
    std::cout << bufferviewLoc << " Bufferview loc\n";
    int vertexCount = accessor.count;
    std::cout << vertexCount << " vertex count\n";

    tinygltf::BufferView bufferView = model.bufferViews[bufferviewLoc];
    int bufferLoc = bufferView.buffer;
    std::cout << bufferLoc << " Buffer loc\n";
    int byteOffset = bufferView.byteOffset;
    int byteLength = bufferView.byteLength;

    tinygltf::Buffer buffer = model.buffers[bufferLoc];

    std::cout << buffer.data.size() << " Buffer size\n";

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());
    
    std::cout << data.size() << " Data size\n";

    data = buffer.data;

    std::cout << data.size() << " Buffer copied to data\n";

    std::vector<float> verts;
    verts.resize(byteLength / sizeof(float));
    std::cout << byteLength / sizeof(float) << " Resize\n";

    std::cout << "Copy data\n";
    std::cout << byteOffset << " offset\n";
    std::cout << byteLength << " length\n";

    std::memcpy(verts.data(), data.data() + byteOffset, byteLength);
    
    std::cout << "Getting index\n";
    int indexAccessorLoc = primitive.indices;
    std::cout << "Got location\n";
    tinygltf::Accessor indexAccessor = model.accessors[indexAccessorLoc];
    std::cout << "Got accessor\n";
    int indexCount = indexAccessor.count;
    std::cout << "Got count\n";
    std::cout << indexCount << '\n';

    int indexBufferviewLoc = indexAccessor.bufferView;
    tinygltf::BufferView indexBufferview = model.bufferViews[indexBufferviewLoc];
    int indexBufferLoc = indexBufferview.buffer;
    int indexByteOffset = indexBufferview.byteOffset;
    int indexBytelength = indexBufferview.byteLength;

    std::cout << "Resize time\n";

    std::vector<unsigned short> inds;
    inds.resize(indexBytelength / sizeof(unsigned short));

    std::cout << "Copy time\n";

    std::memcpy(inds.data(), data.data() + indexByteOffset, indexBytelength);

    // std::memcpy()

    std::cout << "Print time\n";

    for (int i{}; i < inds.size(); ++i){
        std::cout << inds[i] << ' ';
    }
    std::cout << '\n';    

    // Load opengl objects
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, byteLength, data.data() + byteOffset, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytelength, data.data() + indexByteOffset, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GL_UNSIGNED_SHORT), inds.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    */

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}