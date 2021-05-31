#ifndef TRIANGLE_INSTANCES_INCLUDED
#define TRIANGLE_INSTANCES_INCLUDED

#include <AGL/AGL3Window.hpp>
#include <AGL/AGL3Drawable.hpp>

#include <Main/Misc.hpp>
#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


class TriangleInstances : public AGLDrawable {
public:
    glm::vec3* pos;
    glm::vec3* scale;
    glm::vec3* rotations;

    TriangleInstances(const size_t& N);
    void setShaders();
    void setBuffers();
    void draw(glm::mat4& MVP);
    void setColor(float r, float g, float b);
    size_t sizeInstances;
 private:
    static const GLfloat g_vertex_buffer_data[];
    
    GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
};

#endif