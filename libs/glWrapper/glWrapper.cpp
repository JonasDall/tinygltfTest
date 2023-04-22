#include "glWrapper.hpp"
#include "../tinygltf/stb_image.h"

// 
// *SHADER
// 

static std::string ExtractFile(std::string path) // Extracts the contents of a file to a string
{
    std::ifstream file;
    std::string line;
    std::string returnValue;
    file.exceptions(/*std::ifstream::failbit |*/ std::ifstream::badbit); // Set fstream exceptions (failbit removed for unneccesary exceptions)

    try
    {
        file.open(path);

        while(std::getline(file, line)){ // Extract lines
        returnValue += line + '\n';
        }
    }
    catch(std::ifstream::failure e)
    {
        std::cout << e.what() << '\n';
    }
    
    return returnValue; // Return string
}

static void CreateShader(unsigned int &id, GLenum type, std::string code){
    id = glCreateShader(type);
    const char* source = code.c_str();

    glShaderSource(id, 1, &source, NULL);

    glCompileShader(id);

    int success; // Error handle type
    char log[512]; // Error message
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(id, 512, NULL, log);
        std::cout << "FAILED COMPILE: " << log << '\n';
    }

    return;
}

std::vector<float> GetAttributeData(tinygltf::Model& model, tinygltf::Primitive& primitive, std::string target){

    tinygltf::BufferView view = model.bufferViews[model.accessors[primitive.attributes.at(target)].bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;

    tinygltf::Buffer buffer = model.buffers[view.buffer];

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());

    std::vector<float> attributeData;
    attributeData.resize(byteLength / sizeof(float));

    std::memcpy(attributeData.data(), data.data() + byteOffset, byteLength);

    return attributeData;
}

std::vector<unsigned short> GetIndexData(tinygltf::Model& model, tinygltf::Primitive& primitive){

    tinygltf::BufferView view = model.bufferViews[model.accessors[primitive.indices].bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;

    tinygltf::Buffer buffer = model.buffers[view.buffer];

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());

    std::vector<unsigned short> indices;
    indices.resize(byteLength / sizeof(unsigned short));

    std::memcpy(indices.data(), data.data() + byteOffset, byteLength);

    return indices;
}

void CreateGlObjects(glWrap::Primitive &primitive){

    glGenVertexArrays(1, &primitive.m_VAO);
    glGenBuffers(1, &primitive.m_VBO);
    glGenBuffers(1, &primitive.m_EBO);

    glBindVertexArray(primitive.m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, primitive.m_VBO);
    glBufferData(GL_ARRAY_BUFFER, primitive.m_vertices.size() * sizeof(glWrap::Vertex), primitive.m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.m_indices.size() * sizeof(GL_UNSIGNED_SHORT), primitive.m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

glWrap::Shader::Shader(std::string vertexPath, std::string fragmentPath){
    
    unsigned int vertex, fragment; // Shader objects

    CreateShader(vertex, GL_VERTEX_SHADER, ExtractFile(vertexPath));
    CreateShader(fragment, GL_FRAGMENT_SHADER, ExtractFile(fragmentPath));


    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);

    glLinkProgram(m_ID);

    int success; // Error handle type
    char log[512]; // Error message
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, log);
        std::cout << "FAILED LINK: " << log << '\n';
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return;
}

void glWrap::Shader::Use(){
    glUseProgram(m_ID);
}

void glWrap::Shader::SetBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void glWrap::Shader::SetInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetMatrix4(const std::string &name, glm::mat4 mat) const{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 
// *TEXTURE
// 

static GLenum GetChannelType(unsigned int channels){
    switch(channels)
    {
        case 1:
        return GL_RED;

        case 2:
        return GL_RG;

        case 3:
        return GL_RGB;

        case 4:
        return GL_RGBA;
    }

    return GL_RGB;
}

glWrap::Texture2D::Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels){
    stbi_set_flip_vertically_on_load(flip);
    int width, height, channels;
    unsigned char *data = stbi_load(image.c_str(), &width, &height, &channels, 0);

    // std::cout << channels << " channels\n";

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, desiredChannels, width, height, 0, GetChannelType(channels), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else std::cout << "Texture not loaded correctly\n";

    stbi_image_free(data);
}

void glWrap::Texture2D::SetActive(unsigned int unit){
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

std::vector<glWrap::Mesh> glWrap::loadModel(std::string path){
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string error{};
    std::string warning{};

    if (!loader.LoadASCIIFromFile(&model, &error, &warning, "assets/Triangle.gltf"))
    {
        std::cout << error << " | " << warning << '\n';

        std::vector<Mesh> empty;
        return empty;
    }

    std::vector<float> position = GetAttributeData(model, model.meshes[0].primitives[0], "POSITION");
    std::vector<float> normal = GetAttributeData(model, model.meshes[0].primitives[0], "NORMAL");
    std::vector<float> texCoord = GetAttributeData(model, model.meshes[0].primitives[0], "TEXCOORD_0");

    int floats = position.size() + normal.size() + texCoord.size();
    std::vector<Vertex> vertices;
    vertices.resize(floats / 8);

    for (int i{}; i < vertices.size(); ++i){
        int posLoc = i * 3;
        int texLoc = i * 2;

        vertices[i].pos.x = position[0 + posLoc];
        vertices[i].pos.y = position[1 + posLoc];
        vertices[i].pos.z = position[2 + posLoc];
        vertices[i].nor.x = normal[0 + posLoc];
        vertices[i].nor.y = normal[1 + posLoc];
        vertices[i].nor.z = normal[2 + posLoc];
        vertices[i].tex.x = texCoord[0 + posLoc];
        vertices[i].tex.y = texCoord[1 + posLoc];
    }

    std::vector<unsigned short> indices = GetIndexData(model, model.meshes[0].primitives[0]);

}