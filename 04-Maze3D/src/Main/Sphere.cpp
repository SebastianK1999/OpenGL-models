#include "Sphere.hpp"

#include <vector>
#include <cmath>
#include <math.h>


Sphere::Sphere(int N):
    g_vertex_buffer_data(generateMesh(N)),
    pos(0,0,0),
    scale(1,1,1)
{
    GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
    setShaders();
    setBuffers();
}

Sphere::Sphere():Sphere(10){}

std::vector<GLfloat> Sphere::generateMesh(int N){

    GLfloat radius = 1;
    // clear memory of prev arrays
    std::vector<GLfloat> vertices;

                              // vertex texCoord

    GLfloat sectorStep = (GLfloat) M_PI /(GLfloat) N;
    GLfloat stackStep = (GLfloat) M_PI / (GLfloat) N;

    for(int i = 0; i <= N*2; ++i)
    {
        GLfloat stackAngle = i * stackStep;

        for(int j = 0; j <= N*2; ++j)
        {
            GLfloat sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            vertices.push_back(radius * cosf(stackAngle) * cos(sectorAngle));
            vertices.push_back(radius * sinf(stackAngle));
            vertices.push_back(radius * cosf(stackAngle) * sin(sectorAngle));

            vertices.push_back(radius * cosf(stackAngle + stackStep) * cos(sectorAngle));
            vertices.push_back(radius * sinf(stackAngle + stackStep));
            vertices.push_back(radius * cosf(stackAngle + stackStep) * sin(sectorAngle));

            vertices.push_back(radius * cosf(stackAngle) * cos(sectorAngle + sectorStep));
            vertices.push_back(radius * sinf(stackAngle));
            vertices.push_back(radius * cosf(stackAngle) * sin(sectorAngle + sectorStep));

            vertices.push_back(radius * cosf(stackAngle) * cos(sectorAngle + sectorStep));
            vertices.push_back(radius * sinf(stackAngle));
            vertices.push_back(radius * cosf(stackAngle) * sin(sectorAngle + sectorStep));

            vertices.push_back(radius * cosf(stackAngle + stackStep) * cos(sectorAngle));
            vertices.push_back(radius * sinf(stackAngle + stackStep));
            vertices.push_back(radius * cosf(stackAngle + stackStep) * sin(sectorAngle));

            vertices.push_back(radius * cosf(stackAngle + stackStep) * cos(sectorAngle + sectorStep));
            vertices.push_back(radius * sinf(stackAngle + stackStep));
            vertices.push_back(radius * cosf(stackAngle + stackStep) * sin(sectorAngle + sectorStep));
        }
    }

    return vertices;
}

void Sphere::setShaders() {
    compileShaders(R"END(

        #version 330 core
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require

        layout(location = 0) in vec3 vertexPosition_modelspace;
        layout(location = 0) uniform vec4 pos;
        layout(location = 1) uniform vec4 scale;

        layout(location = 2)uniform mat4 MVP;

        out vec3 vpos;

        void main(){	
            gl_Position =  MVP * (vec4(vertexPosition_modelspace,1) * scale + pos) ;
            vpos = vertexPosition_modelspace;
        }
    )END", R"END(

        #version 330 core

        in vec3 vpos;

        out vec3 color;

        void main(){
            color = vec3(0, 0,(vpos.y  + 1) * 0.4);

        }
    )END");
}

void Sphere::setBuffers() {
    bindBuffers();

    GLuint vertexbuffer;    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * g_vertex_buffer_data.size(), g_vertex_buffer_data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
}

void Sphere::draw(glm::mat4& MVP) {
    bindProgram();
    bindBuffers();
    glUniform4f(0, pos[0],pos[1],pos[2],0);
    glUniform4f(1, scale[0],scale[1],scale[2],1);
    glUniformMatrix4fv(2, 1, GL_FALSE, &MVP[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size()); // 12*3 indices starting at 0 -> 12 triangles
}

void Sphere::setColor(float r, float g, float b){
    cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
}