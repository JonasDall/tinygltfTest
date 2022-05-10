#include <iostream>
#include "gameStuff.hpp"
#include "glfw3.h"

int main(){

gs::Object a(43);

std::cout << a.getIndex() << '\n';

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

while (!glfwWindowShouldClose(window))
{
    //Loop
}

glfwDestroyWindow(window);
glfwTerminate();

return 0;
}