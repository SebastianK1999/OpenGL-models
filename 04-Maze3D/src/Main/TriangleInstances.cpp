#include "TriangleInstances.hpp"

//first and las tolumn are swaped, now culling shows only inside of cube;
const GLfloat TriangleInstances::g_vertex_buffer_data[] = { 
     0.0f,1.0f,0.0f,
     1.0f * sinf(M_PI * 2/3),1.0f * cosf(M_PI * 2/3),0.0f,
     1.0f * sinf(M_PI * 4/3),1.0f * cosf(M_PI * 4/3),0.0f
};
TriangleInstances::TriangleInstances(const size_t& N) : 
    sizeInstances(N*N*N-1)
{
    pos = new glm::vec3[sizeInstances];
    for(int i = 1; i<N*N*N; i++){
        pos[i - 1] = glm::vec3((i%N)  /(N/2.0f)-(1 - 1.0f/N),(i/N/N)  /(N/2.0f)-(1 - 1.0f/N),(i%(N*N))/N /(N/2.0f)-(1 - 1.0f/N));
    }

    scale = new glm::vec3[sizeInstances];
    for(int i = 1; i<N*N*N; i++){
        GLfloat s = (((i%N)  /(N*N/2.0f)) + ((i/N/N)  /(N*N/4.0f * 3)) + ((i%(N*N))/N /(N*N/2.0f))) / 3;
        scale[i - 1] = glm::vec3(s,s,s);
    }

    rotations = new glm::vec3[sizeInstances];
    for(int i = 1; i<N*N*N; i++){
        rotations[i - 1] = glm::vec3(Misc::rand::get().Decim(M_PI*2),Misc::rand::get().Decim(M_PI*2),Misc::rand::get().Decim(M_PI*2));
    }
    
    GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
    setShaders();
    setBuffers();

}

void TriangleInstances::setShaders() {
    compileShaders(R"END(

        #version 330 core
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require

        layout(location = 0) in vec3 vertexPosition_modelspace;
        layout(location = 1) in vec3 offsetPos;
        layout(location = 2) in vec3 offsetScale;
        layout(location = 3) in vec3 offsetRotation;

        layout(location = 0)uniform mat4 MVP;

        out vec3 vpos;

        void main(){
            mat3 rot_x = {{1,0,0},
                        {0,cos(offsetRotation[0]),-sin(offsetRotation[0])},
                        {0,sin(offsetRotation[0]),cos(offsetRotation[0])}};
            mat3 rot_y = {{cos(offsetRotation[1]),0,sin(offsetRotation[1])},
                        {0,1,0},
                        {-sin(offsetRotation[1]),0,cos(offsetRotation[1])}};
            mat3 rot_z = {{cos(offsetRotation[2]),-sin(offsetRotation[2]),0},
                          {sin(offsetRotation[2]), cos(offsetRotation[2]) ,0},
                          {0,0,1}};

            //vec4 pos = (vec4(gl_InstanceID%10,gl_InstanceID/100,(gl_InstanceID%100)/10,0)/5.0 - vec4(0.9,0.9,0.9,0));
            gl_Position =  MVP * (vec4(rot_z * (rot_y * (rot_x * vertexPosition_modelspace)),1) * vec4(offsetScale, 1) + vec4(offsetPos,0));
            vpos = offsetPos;
        }
    )END", R"END(

        #version 330 core

        in vec3 vpos;

        out vec3 color;

        void main(){
            color = vpos;

        }
    )END");
}

void TriangleInstances::setBuffers() {
    bindBuffers();


    GLuint vertexbuffer;    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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


    unsigned int posVBO;
    glGenBuffers(1, &posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeInstances, &pos[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*3, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glVertexAttribDivisor(1,1);

    unsigned int sizeVBO;
    glGenBuffers(1, &sizeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeInstances, &scale[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*3, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glVertexAttribDivisor(2,1);

    unsigned int rotationVBO;
    glGenBuffers(1, &rotationVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeInstances, &rotations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*3, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glVertexAttribDivisor(3,1);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void TriangleInstances::draw(glm::mat4& MVP) {
    bool enableCulling = 0;
    if(glIsEnabled(GL_CULL_FACE)){
        enableCulling = 1;
        glDisable(GL_CULL_FACE); 
    }
    bindProgram();
    bindBuffers();
    glUniformMatrix4fv(0, 1, GL_FALSE, &MVP[0][0]);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, sizeInstances); // 12*3 indices starting at 0 -> 12 triangles
    if(enableCulling){
        glEnable(GL_CULL_FACE);
    }
}

void TriangleInstances::setColor(float r, float g, float b){
    cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
}