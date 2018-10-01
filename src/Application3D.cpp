/**
 * Application3D.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */
#include "Application3D.hpp"
#include <vector>

#include <glm/glm.hpp>
#include <iostream>
#include "utils/glError.hpp"

#include <GLFW/glfw3.h>

Application3D::Application3D():
    Application()
{
    glCheckError(__FILE__,__LINE__);
}



void Application3D::loop()
{
}
