// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGL3 example usage 
//===========================================================================
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>

// ==========================================================================
// Drawable object: no-data only: vertex/fragment programs
// ==========================================================================
class MyTri : public AGLDrawable {
public:
   MyTri() : AGLDrawable(0) {
      setShaders();
   }
   void setShaders() {
            compileShaders(R"END(

         #version 330 
         out vec4 vcolor;
         out vec2 vpos;
         void main(void) {
            const vec2 vertices[] = vec2[3](vec2( 0.9, -0.9),
                                             vec2(-0.9, -0.9),
                                             vec2( 0.9,  0.9));
            const vec4 colors[]    = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),
                                             vec4(0.0, 1.0, 0.0, 1.0),
                                             vec4(0.0, 0.0, 1.0, 1.0));

            vcolor      = colors[gl_VertexID];
            gl_Position = vec4(vertices[gl_VertexID], 0.5, 1.0); 
            vpos = vertices[gl_VertexID];
         }

      )END", R"END(

         #version 330 
         
         in  vec4 vcolor;
         in  vec2 vpos;
         out vec4 color;

         vec2 coord = vpos;

         void main(void) {
            color = vcolor;
            if(color != vec4(0)){ color += ((0.9 - abs(coord.x)) * (0.9 - abs(coord.y)) ); }
         } 

      )END");
   }
   void draw() {
      bindProgram();
      glDrawArrays(GL_TRIANGLES, 0, 3);
   }
};


// ==========================================================================
// Drawable object with some data in buffer and vertex/fragment programs
// ==========================================================================
class MyCross : public AGLDrawable {
public:
   double_t scale;
   double_t x;
   double_t y;

   MyCross(double_t X, double_t Y) : AGLDrawable(0), scale(1.0/16.0), x(X), y(Y){
      setShaders();
      setBuffers();
   }
   void setShaders() {
      compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         layout(location = 0) in vec2 pos;
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         out vec4 vtex;

         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

      )END", R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         layout(location = 3) uniform vec3  cross_color;
         out vec4 color;

         void main(void) {
            color = vec4(cross_color,1.0);
         } 

      )END");
   }
   void setBuffers() {
      bindBuffers();
      GLfloat vert[4][2] = {  // Cross lines
         { -1,  0  },
         {  1,  0  },
         {  0, -1  },
         {  0,  1  }
      };

      glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), vert, GL_STATIC_DRAW );
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
         0,                 // attribute 0, must match the layout in the shader.
         2,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,//24,             // stride
         (void*)0            // array buffer offset
      );
   }
   void draw() {
      bindProgram();
      bindBuffers();
      glUniform1f(0, this->scale);  // scale  in vertex shader
      glUniform2f(1, this->x, this->y);  // center in vertex shader
      glUniform3f(3, cross_color[0],cross_color[1],cross_color[2]);

      glDrawArrays(GL_LINES, 0, 4);
   }
   void setColor(float r, float g, float b){
      cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
   }
 private:
   GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
};

// ==========================================================================
// Drawable object with some data in buffer and vertex/fragment programs
// ==========================================================================
class MyCircle : public AGLDrawable {
public:
   size_t n;
   double_t scale;
   double_t x;
   double_t y;

   MyCircle(double_t X, double_t Y) : AGLDrawable(0), scale(1.0/16.0), x(X), y(Y){
      this->n=16;
      setShaders();
      setBuffers();

   }
   void setShaders() {
      compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         layout(location = 0) in vec2 pos;
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         out vec4 vtex;

         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

      )END", R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         layout(location = 3) uniform vec3  circle_color;
         out vec4 color;

         void main(void) {
            color = vec4(circle_color,1.0);
         } 

      )END");
   }
   void setBuffers() {
      bindBuffers();
      GLfloat vert[this->n*2][2] = { };
      double jump = 2 * M_PI / this->n;
      for(size_t i = 0; i < n;i++){
         size_t I = i*2;
         vert[I][0] = std::cos(jump*i);
         vert[I][1] = std::sin(jump*i);
         vert[I+1][0] = std::cos(jump*(i+1));
         vert[I+1][1] = std::sin(jump*(i+1));
      }

      glBufferData(GL_ARRAY_BUFFER, this->n*4*sizeof(float), vert, GL_STATIC_DRAW );
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
         0,                 // attribute 0, must match the layout in the shader.
         2,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,//24,             // stride
         (void*)0            // array buffer offset
      );
   }
   void draw() {
      bindProgram();
      bindBuffers();
      glUniform1f(0, this->scale);  // scale  in vertex shader
      glUniform2f(1, this->x, this->y);  // center in vertex shader
      glUniform3f(3, circle_color[0],circle_color[1],circle_color[2]);

      glDrawArrays(GL_LINES, 0, this->n*2);
   }
   void setColor(float r, float g, float b){
      circle_color[0]=r;circle_color[1]=g;circle_color[2]=b;
   }
 private:
   GLfloat circle_color[3] = { 1.0, 0.0, 0.0 };
};

// ==========================================================================
// Window Main Loop Inits ...................................................
// ==========================================================================
class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
        : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    virtual void KeyCB(int key, int scancode, int action, int mods);
    void MainLoop();
};

// ==========================================================================
void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
       ; // do something
    }
    if (key == GLFW_KEY_HOME  && (action == GLFW_PRESS)) {
       ; // do something
    }
}


// ==========================================================================
void MyWin::MainLoop() {
   ViewportOne(0,0,wd,ht);

   MyCross  cross( 0,  0.5);
   MyCircle circle(0, -0.5);
   MyTri   trian;

   do {
      glClear( GL_COLOR_BUFFER_BIT );
   
      AGLErrors("main-loopbegin");
      // =====================================================        Drawing
      trian.draw();
      cross.draw();
      circle.draw();
      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win()); // =============================   Swap buffers
      glfwPollEvents();
      //glfwWaitEvents();   

      if (glfwGetKey(win(), GLFW_KEY_DOWN ) == GLFW_PRESS) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      if (glfwGetKey(win(), GLFW_KEY_UP ) == GLFW_PRESS) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
      if (glfwGetKey(win(), GLFW_KEY_RIGHT ) == GLFW_PRESS) {
         cross.x += 0.01;
      }
      if (glfwGetKey(win(), GLFW_KEY_LEFT ) == GLFW_PRESS) {
         cross.x -= 0.01;
      }
      if(glfwGetKey(win(), GLFW_KEY_W ) == GLFW_PRESS){
         circle.y += 0.01;
      }
      if(glfwGetKey(win(), GLFW_KEY_S ) == GLFW_PRESS){
         circle.y -= 0.01;
      }
      if(glfwGetKey(win(), GLFW_KEY_A ) == GLFW_PRESS){
         circle.x -= 0.01;
      }
      if(glfwGetKey(win(), GLFW_KEY_D ) == GLFW_PRESS){
         circle.x += 0.01;
      }
      
      if(pow(abs(cross.x - circle.x),2) + pow(abs(cross.y - circle.y),2) < pow((circle.scale),2)){ // lub pow((cross,scale + circle.scale),2) w zależności od egektu jaki chcemy
         break;
      }


   } while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
}

int main(int argc, char *argv[]) {
   MyWin win;
   win.Init(800,800,"AGL3 example",0,33);
   win.MainLoop();
   return 0;
}
