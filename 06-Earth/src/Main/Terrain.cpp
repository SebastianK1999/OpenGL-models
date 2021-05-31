#include "Terrain.hpp"

#define GLOBE_SCALE 1.0/6371000.0

#include <vector>
#include <cmath>
#include <math.h>


Terrain::Terrain():
    g_vertex_buffer_data(generateMesh(0, 0, 3)),
    pos(0,0,0),
    scale(1,1,1),
    endThread(false),
    curCamPos(0,0,1),
    loadBuff(false)
{
    GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
    setShaders();
    setBuffers();
    this->thread = std::thread(&Terrain::threadLoop, this);

    std::vector<unsigned int> index_buffer;
    index_buffer.reserve(6*149*149);
    for(int i = 0; i < 149; i++){
        for(int j = 0; j < 149; j++){
            index_buffer.emplace_back(i    *150+j+1);
            index_buffer.emplace_back((i+1)*150+j  );
            index_buffer.emplace_back(i    *150+j  );

            index_buffer.emplace_back((i+1)*150+j);
            index_buffer.emplace_back(i*150+(j+1));
            index_buffer.emplace_back((i+1)*150+j+1);

        }
    }

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer.size() * sizeof(unsigned int), &index_buffer[0] , GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

Terrain::~Terrain(){
    static std::unique_lock<std::mutex> lc(endThread_mux, std::defer_lock);
    lc.lock();
    this->endThread = true;
    lc.unlock();
    this->thread.join();
}

void Terrain::threadLoop(){
    std::cout << "Starting thred\n";
    static std::unique_lock<std::mutex> lc_cam(this->curCamPos_mux, std::defer_lock);
    static std::unique_lock<std::mutex> lc_end(this->endThread_mux, std::defer_lock);
    static std::unique_lock<std::mutex> lc_buff(this->loadBuff_mux, std::defer_lock);

    std::vector<GLfloat> vert;
    std::vector<GLfloat> normal;

    glm::vec3 camPos(0,0,3);
    glm::vec3 lastCamPos(0,0,3);

    lc_end.lock();
    while(!this->endThread){
        lc_end.unlock();

        lc_cam.lock();
        camPos = this->curCamPos;
        lc_cam.unlock();
        
        float radius = glm::length(camPos);
        //float stackRadius = std::sqrt(camPos.z*camPos.z + camPos.x*camPos.x);
        float verRad = std::asin(camPos.y/radius);
        float horRad = std::asin(camPos.x/(cosf(verRad)*radius));
        if(camPos.z < 0) horRad = M_PI - horRad;
        //std::cout << (radius - 1)<< " " << glm::length(camPos - lastCamPos) << "\n";

        float sensitiy = 0.1;
        if(glm::length(lastCamPos) < glm::length(camPos)) sensitiy = 0.05;

        if(glm::length(camPos - lastCamPos) > sensitiy*(radius - 1)){
            //std::cout << verRad << " " << horRad << "\n";
            lastCamPos = camPos;
            vert = generateMesh(verRad,horRad, radius);

            lc_buff.lock();
            this->g_vertex_buffer_data = vert;
            this->loadBuff = true;
            lc_buff.unlock();

        }
        lc_end.lock();
    }
    std::cout << "Ending thread\n";
    
}

void Terrain::refresh(glm::vec3 camPos){
    static std::unique_lock<std::mutex> lc_cam(this->curCamPos_mux, std::defer_lock);
    static std::unique_lock<std::mutex> lc_buff(this->loadBuff_mux, std::defer_lock);
        
        lc_cam.lock();
        this->curCamPos = camPos;
        lc_cam.unlock();
        lc_buff.lock();
        if(this->loadBuff){
            setBuffers();
            this->loadBuff = false;
        }
        lc_buff.unlock();

}

std::vector<GLfloat> Terrain::generateMesh(float verRad, float horRad, float radius){
    static FileLoader fl;
    const int fRes = 1201;
    
    int horIter = 150;
    int verIter = 150;
    int stepInedx = 500 * (radius - 1);

    int16_t height_00;
    //int16_t height_01;
    //int16_t height_10;
    //int16_t height_11;

    size_t deg_0x;
    //size_t deg_1x;
    size_t deg_x0;
    //size_t deg_x1;
    size_t degIndex_0x;
    //size_t degIndex_1x; 
    size_t degIndex_x0; 
    //size_t degIndex_x1; 

    float radius_00;
    //float radius_01;
    float radius_10;
    //float radius_11;

    float sinf_0x;
    float cosf_0x;
    //float sinf_x1;
    //float cosf_x1;
    //float sinf_1x;
    //float cosf_1x;
    float sinf_x0;
    float cosf_x0;


    std::vector<GLfloat> vertices;
    //vertices.reserve(verIter*horIter*6);
    vertices.reserve(verIter*horIter*3);

    float step = (3.1415193 / 180.0) / (fRes-1);

    int startStack = floor(verRad/step) - (verIter/2 * stepInedx);
    int startSector = floor(horRad/step) - (horIter/2 * stepInedx);

    const GLfloat heightScale = GLOBE_SCALE;
    for(int i = 0; i < verIter; i++){
        int stack = startStack + i*stepInedx;
        GLfloat stackAngle = step * stack; 
        for(int j = 0; j < horIter; j++){
            int sector = startSector + j*stepInedx;
            GLfloat sectorAngle = step * sector;

            deg_0x      = static_cast<size_t>( floor(static_cast<float>(stack)/fRes)            + 90)%180;
            //deg_1x      = static_cast<size_t>(floor((static_cast<float>(stack)+stepInedx)/fRes) + 90)%180;
            deg_x0      = static_cast<size_t>(floor( static_cast<float>(sector)/fRes            + 180))%360;
            //deg_x1      = static_cast<size_t>(floor((static_cast<float>(sector)+stepInedx)/fRes + 180))%360;
            degIndex_0x = (fRes*(fRes - (stack%fRes)))%(fRes*fRes);
            //degIndex_1x = (fRes*(fRes - (stack+stepInedx)%fRes))%(fRes*fRes);
            degIndex_x0 = sector%fRes;
            //degIndex_x1 = (sector+stepInedx)%fRes;

            //((fRes*(fRes - (stack+stepInedx)%fRes)))%(fRes*fRes)

            fl.getHeight(height_00, deg_0x, deg_x0, degIndex_0x, degIndex_x0);
            //fl.getHeight(height_01, deg_0x, deg_x1, degIndex_0x, degIndex_x1);
            //fl.getHeight(height_10, deg_1x, deg_x0, degIndex_1x, degIndex_x0);
            //fl.getHeight(height_11, deg_1x, deg_x1, degIndex_1x, degIndex_x1);

            //radius_10 = 1.0 + heightScale * height_10;
            //radius_01 = 1.0 + heightScale * height_01;
            radius_00 = 1.0 + heightScale * height_00; 
            //radius_11 = 1.0 + heightScale * height_11;

            sinf_0x = sinf(stackAngle);
            cosf_0x = cosf(stackAngle);
            //sinf_x1 = sinf(sectorAngle + step*stepInedx);
            //cosf_x1 = cosf(sectorAngle + step*stepInedx);
            //sinf_1x = sinf(stackAngle + step*stepInedx);
            //cosf_1x = cosf(stackAngle + step*stepInedx);
            sinf_x0 = sinf(sectorAngle);
            cosf_x0 = cosf(sectorAngle);
            
            //vertices.emplace_back(radius_01 * cosf_0x * sinf_x1);
            //vertices.emplace_back(radius_01 * sinf_0x);
            //vertices.emplace_back(radius_01 * cosf_0x * cosf_x1);

            //vertices.emplace_back(radius_10 * cosf_1x * sinf_x0);
            //vertices.emplace_back(radius_10 * sinf_1x);
            //vertices.emplace_back(radius_10 * cosf_1x * cosf_x0);

            vertices.emplace_back(radius_00 * cosf_0x * sinf_x0);
            vertices.emplace_back(radius_00 * sinf_0x);
            vertices.emplace_back(radius_00 * cosf_0x * cosf_x0);

            // reuse 10
            //vertices.emplace_back(vertices[vertices.size()-6]);
            //vertices.emplace_back(vertices[vertices.size()-6]);
            //vertices.emplace_back(vertices[vertices.size()-6]);
            // reuse 01
            //vertices.emplace_back(vertices[vertices.size()-12]);
            //vertices.emplace_back(vertices[vertices.size()-12]);
            //vertices.emplace_back(vertices[vertices.size()-12]);

            //vertices.emplace_back(radius_11 * cosf_1x * sinf_x1);
            //vertices.emplace_back(radius_11 * sinf_1x);
            //vertices.emplace_back(radius_11 * cosf_1x * cosf_x1);

        }
    }
    return vertices;
}

void Terrain::setShaders() {
    compileShaders(R"END(

        #version 330 core
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require

        layout(location = 0) in vec3 vertexPosition_modelspace;
        layout(location = 0) uniform vec4 pos;
        layout(location = 1) uniform vec4 scale;
        layout(location = 2) uniform vec3 light;

        layout(location = 3)uniform mat4 MVP;

        out vec3 vpos;
        out vec3 vcol;

        void main(){
            vec3 pos3 = vec3(pos[0],pos[1],pos[2]);
            float R = length(pos3 - light);
            vec3 sc = vec3(scale[0],scale[1],scale[2]);
            vec3 L = (pos3 - light)/R;	
            vec3 normal = 3*vertexPosition_modelspace/vertexPosition_modelspace.length(); 
            
            gl_Position =  MVP * (vec4(vertexPosition_modelspace,1) * scale + pos);

            const float gHgt = 500 / 6371000.0;
            const float yHgt = 1000 / 6371000.0;
            const float rHgt = 2000 / 6371000.0;
            const float hgt = length(vertexPosition_modelspace) - 1;

            vec3 liCol = vec3(0);
            if     (hgt <= 0   )    liCol = vec3( 0.0,           0.0,           1.0          );
            else if(hgt <  gHgt)    liCol = vec3( 0.0,           hgt/gHgt,      0.0          );
            else if(hgt <  yHgt)    liCol = vec3( hgt/gHgt - 1,  1,             0.0          );
            else if(hgt <  rHgt)    liCol = vec3( 1,             2.0-hgt/yHgt,  0.0          );
            else                    liCol = vec3( 1,             hgt/rHgt - 1,  hgt/rHgt - 1 );

            float r = R;
            if(r > 1){
                r = 1.0;
            }
            if(length(pos3) == 1){
                vcol = vec3(0,0,1);
            }
            else if(length(pos3) > 1){
                vcol = vec3(0,1,0) * (length(pos3) - 1);
            }
            else{
                vcol = vec3(0,0,0);
            }

            if( acos(dot(normal, L) / length(normal) / length(L)) > 3.141593/2.0){
                vcol += liCol * 0.7 * (length(dot(normal,L)));
            }
            else{
                vcol += vec3(0,0,0) * (length(dot(normal,L)));
            }

            vpos = pos3 + sc * vertexPosition_modelspace;
        }
    )END", R"END(

        #version 330 core

        in vec3 vpos;
        in vec3 vcol;

        out vec4 color;

        void main(){
            vec3 ocol = vec3(0,0,0);
            //if(length(vpos) <= 1.0) {ocol = vec3(0,0,1);}

            // aplha blending
            float I = length(vcol) + length(ocol)*(1.0 - length(vcol));
            //ocol = ocol * length(vcol);
            ocol = ( vcol*length(vcol) + ocol*length(ocol) * (1.0 - length(vcol)) ) / I;


            color = vec4(
                ocol[0],
                ocol[1],
                ocol[2],
                1
            );
        }
    )END");
}

void Terrain::setBuffers() {
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

void Terrain::draw(glm::mat4& MVP, glm::vec3& light) {
    bindProgram();
    bindBuffers();
    glUniform4f(0, pos[0],pos[1],pos[2],0);
    glUniform4f(1, scale[0],scale[1],scale[2],1);
    glUniform3f(2, light[0], light[1], light[2]);
    glUniformMatrix4fv(3, 1, GL_FALSE, &MVP[0][0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, 149*149*6, GL_UNSIGNED_INT, (void*)0); // 12*3 indices starting at 0 -> 12 triangles

    //glDisableVertexAttribArray(0);
}

void Terrain::setColor(float r, float g, float b){
    cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
}