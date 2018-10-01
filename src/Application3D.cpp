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
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils/glError.hpp"

#include <GLFW/glfw3.h>

#define SHADER_DIR "./shader/"


struct VertexType
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
};

float heightMap(const glm::vec2 position)
{
    return 2.0*sin(position.x)*sin(position.y);
}

VertexType getHeightMap(const glm::vec2 position)
{
    const glm::vec2 dx(1.0,0.0);
    const glm::vec2 dy(0.0,1.0);

    VertexType v;
    float h = heightMap(position);
    float hx = 100.f * (heightMap(position + 0.01f*dx) - h );
    float hy = 100.f * (heightMap(position + 0.01f*dy) - h );

    v.position = glm::vec3(position,h);
    v.normal = glm::normalize(glm::vec3(-hx,-hy,1.0));

    float c = sin(h*5.f)*0.5+0.5;
    v.color = glm::vec4(c,1.0-c,1.0,1.0);
    return v;
}


Application3D::Application3D():
    Application(),
    vertexShader(SHADER_DIR"/shader.vert",GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR"/shader.frag",GL_FRAGMENT_SHADER),
    shaderProgram({vertexShader,fragmentShader})
{
    glCheckError(__FILE__,__LINE__);
}



void Application3D::loop()
{
}
