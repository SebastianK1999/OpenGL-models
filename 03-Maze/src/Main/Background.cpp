#include "Background.hpp"

Background::Background() : 
    AGLDrawable(0),
    xs{Misc::rand::get().Decim(-0.9,0.9), Misc::rand::get().Decim(-0.9,0.9), Misc::rand::get().Decim(-0.9,0.9)},
    ys{Misc::rand::get().Decim(-0.9,0.9), Misc::rand::get().Decim(-0.9,0.9), Misc::rand::get().Decim(-0.9,0.9)},
    alphas{Misc::rand::get().Decim(2*M_PI), Misc::rand::get().Decim(2*M_PI), Misc::rand::get().Decim(2*M_PI)}
{
    setShaders();
}
void Background::setShaders() {
        compileShaders(R"END(

        #version 330 
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require
        out vec2 vpos;
        void main(void) {
        const vec2 vertices[] = vec2[4](vec2( -0.9, -0.9),
                                            vec2(0.9, -0.9),
                                            vec2( 0.9,  0.9),
                                            vec2( -0.9,  0.9));
        gl_Position = vec4(vertices[gl_VertexID], 0, 1.0); 
        vpos = vertices[gl_VertexID];
        }

    )END", R"END(

        #version 330 
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require
        
        layout(location = 10) uniform vec3  xs;
        layout(location = 11) uniform vec3  ys;

        in  vec2 vpos;
        out vec4 color;

        void main(void) {
            color = vec4(
                (1 - sqrt( pow(vpos.x - xs[0], 2) + pow(vpos.y - ys[0],2) )) * 0.6  ,
                (1 - sqrt( pow(vpos.x - xs[1], 2) + pow(vpos.y - ys[1],2) )) * 0.6  ,
                (1 - sqrt( pow(vpos.x - xs[2], 2) + pow(vpos.y - ys[2],2) )) * 0.6  ,
                1);
        } 

    )END");
}
void Background::draw() {
    bindProgram();
    bindBuffers();
    glUniform3f(10, xs[0],xs[1],xs[2]);
    glUniform3f(11, ys[0],ys[1],ys[2]);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    this->move();
}

void Background::move(){
    for(int i = 0; i < 3; i++ ){
        xs[i] += cos(alphas[i]) * 0.01;
        ys[i] += sin(alphas[i]) * 0.01;
        if(xs[i] > 0.9 || xs[i] < -0.9){
            alphas[i] = M_PI - alphas[i];
        }
        else if(ys[i] > 0.9 || ys[i] < -0.9){
            alphas[i] = - alphas[i];   
        }
    }
}