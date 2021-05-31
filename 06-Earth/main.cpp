#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <chrono>


#include <AGL/AGL3Window.hpp>
#include <AGL/AGL3Drawable.hpp>
#include <Main/Misc.hpp>
#include <Main/Sphere.hpp>
#include <Main/Terrain.hpp>


#include <common/controls.hpp>
#include <glm/glm.hpp>

class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
        : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    virtual void KeyCB(int key, int scancode, int action, int mods);
    void setup();
    static void reshape(GLFWwindow * a, int b,int c);
    int MainLoop(long int grid_size);
};


void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
        ; // do something
    }
    if (key == GLFW_KEY_HOME  && (action == GLFW_PRESS)) {
        ; // do something
    }
}


void MyWin::reshape(GLFWwindow * a, int b,int c) {
        /*if(b < c){
            int rest = (c-b)/2;
            glViewport(0, rest, b, b);
        }
        else{
            int rest = (b-c)/2;
            glViewport(rest, 0, c, c);
        }*/
        glViewport(0, 0, b, c);
}

void MyWin::setup(){
    //ViewportOne(0,0,wd,ht);
    //glfwSetWindowSizeCallback(win(), MyWin::reshape);

    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfwSetInputMode(win(), GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.4f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
}

int MyWin::MainLoop(long int grid_size) {
    std::cout << -1/30 << "\n";
    
    this->setup();

    MovingViewPort camera(win());
    camera.radius = 3;
    camera.speed = 5.0f/grid_size;
    camera.setCoord(0,0);
    bool switchCamera = 0;
    bool vievMainCamera = 1;

    size_t wins = 0;


    Sphere globeBg(15, false);
    Sphere globeLines(18, true);
    globeLines.scale *= 1.003;
    Terrain terra;
 

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 MVP;


    int xsize, ysize;
	glfwGetWindowSize(win(), &xsize, &ysize);
    //glMatrixMode(ProjectionMatrix);

    do {
        auto start_time = std::chrono::high_resolution_clock::now();
        glfwGetWindowSize(win(), &xsize, &ysize);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //allows moving in other view
        camera.computeMatricesFromInputs(globeBg.pos);

        if(vievMainCamera){
            ProjectionMatrix = camera.getProjectionMatrix();
            ViewMatrix = camera.getViewMatrix();
            ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            terra.refresh(camera.position);

            AGLErrors("main-loopbegin");
            globeBg.draw(MVP, camera.position);
            terra.draw(MVP, camera.position);
            globeLines.draw(MVP, camera.position);
        }

        glfwSwapBuffers(win()); // =============================   Swap buffers
        glfwPollEvents();

        if (glfwGetKey( win(), GLFW_KEY_TAB ) == GLFW_PRESS){
			switchCamera = 1;
		}
        if (switchCamera == 1 && glfwGetKey( win(), GLFW_KEY_TAB ) == GLFW_RELEASE){
			vievMainCamera = !vievMainCamera;
            switchCamera = 0;

		}
        auto tim = std::chrono::high_resolution_clock::now() - start_time;
        std::cout<< "fps: "  << 1000.0/(tim/std::chrono::milliseconds(1))<< "\n"; 
    } // end of do
        while( glfwWindowShouldClose(win()) == 0 || camera.active);

    return wins;
}


int main(int argc, char *argv[]) {
    // it let's you set text as seed
    // for some reason then 'w' char is in te arg it ignores the rest of the seed
    /*long int SEED;
    if(argc > 1){ 
        std::istringstream ss(argv[1]);
        if (!(ss >> SEED) || !ss.eof()) {
            SEED = std::strtol(argv[1], nullptr, 32);
            Misc::rand::setSeed(SEED);
        }
        else Misc::rand::setSeed(SEED);
    }
    std::cout << "Current seed: " << SEED << "\n";
    */
    // convert arguntent for grid_size
    int grid_size = 3;
    if(argc > 1){ 
        std::istringstream ss(argv[1]);
        if (!(ss >> grid_size)) {
            std::cerr << "Invalid number: " << argv[1] << '\n';
        } else if (!ss.eof()) {
            std::cerr << "Trailing characters after number: " << argv[1] << '\n';
        }
    }

    MyWin win;
    win.Init(800,800,"Labirynt 3D - 309039",0,33);

    

    int wins =    win.MainLoop(grid_size);

    return 0;
}