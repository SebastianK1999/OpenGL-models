#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <ctime>


#include <AGL/AGL3Window.hpp>
#include <AGL/AGL3Drawable.hpp>
#include <Main/Misc.hpp>
#include <Main/Shapes.hpp>

#include <common/common.hpp>
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
    
    glEnable(GL_CULL_FACE); 
}

int MyWin::MainLoop(long int grid_size) {
    
    this->setup();

    MovingViewPort camera(win());
    camera.radius = 5.0f/grid_size;
    camera.speed = 5.0f/grid_size;

    MovingViewPort camera2(win());

    size_t deaths = 0;

    //Background bg;
    Cube cube;
    TriangleInstances obsticles(grid_size);

    Sphere player(15);
    player.scale[0] = 0.5 / grid_size;
    player.scale[1] = 0.5 / grid_size;
    player.scale[2] = 0.5 / grid_size;
    player.pos[0] = (1.0f/grid_size)-1;
    player.pos[1] = (1.0f/grid_size)-1;
    player.pos[2] = (1.0f/grid_size)-1;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 MVP;
    camera2.computeMatricesFromInputs(); 
    int xsize, ysize;
	glfwGetWindowSize(win(), &xsize, &ysize);
    //glMatrixMode(ProjectionMatrix);
    do {
        glfwGetWindowSize(win(), &xsize, &ysize);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.computeMatricesFromInputs(player.pos);    
        ProjectionMatrix = camera.getProjectionMatrix();
        ViewMatrix = camera.getViewMatrix();
        ModelMatrix = glm::mat4(1.0);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        AGLErrors("main-loopbegin");
        glClear(GL_DEPTH_BUFFER_BIT); 
        
        glViewport(0, 0, xsize, ysize);
        glScissor(0, 0, xsize, ysize);
        // =====================================================        Drawing
        player.draw(MVP);
        cube.draw(MVP);
        obsticles.draw(MVP);
        AGLErrors("main-afterdraw");
        AGLErrors("main-loopbegin");

        ProjectionMatrix = camera2.getProjectionMatrix();
        ViewMatrix = camera2.getViewMatrix();
        ModelMatrix = glm::mat4(1.0);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


        glClear(GL_DEPTH_BUFFER_BIT); 
        glViewport(xsize - ysize/3, 0, ysize/3, ysize/3);
        glScissor( xsize - ysize/3, 0, ysize/3, ysize/3);
        // =====================================================        Drawing
        cube.draw(MVP);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        player.draw(MVP);
        obsticles.draw(MVP);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        AGLErrors("main-afterdraw");


        glfwSwapBuffers(win()); // =============================   Swap buffers
        glfwPollEvents();
        if( pow(obsticles.pos[obsticles.sizeInstances-1][0] - player.pos[0],2) + pow(obsticles.pos[obsticles.sizeInstances-1][1] - player.pos[1],2) + pow(obsticles.pos[obsticles.sizeInstances-1][2] - player.pos[2],2)
                <
                pow(player.scale[0],2) + pow(player.scale[1],2) + pow(player.scale[2],2)
            ){
            break;
        }       

        // Colisions
        if(abs(player.pos[0]) + player.scale[0] > 1) player.pos[0] = (player.pos[0] > 0 ? 1 - player.scale[0] : -1 + player.scale[0]);
        if(abs(player.pos[1]) + player.scale[1] > 1) player.pos[1] = (player.pos[1] > 0 ? 1 - player.scale[1] : -1 + player.scale[1]);
        if(abs(player.pos[2]) + player.scale[2] > 1) player.pos[2] = (player.pos[2] > 0 ? 1 - player.scale[2] : -1 + player.scale[2]);
        for(size_t i = 0; i < obsticles.sizeInstances; i++){ // I spend to mutch on this "project". here are some awful colisions
            if( pow(obsticles.pos[i][0] - player.pos[0],2) + pow(obsticles.pos[i][1] - player.pos[1],2) + pow(obsticles.pos[i][2] - player.pos[2],2)
                <
                pow(player.scale[0],2) + pow(player.scale[1],2) + pow(player.scale[2],2)
            ){
                player.pos[0] = (player.pos[0] > obsticles.pos[i][0] ? obsticles.pos[i][0] + player.scale[0] : obsticles.pos[i][0] - player.scale[0]);
                player.pos[1] = (player.pos[1] > obsticles.pos[i][1] ? obsticles.pos[i][1] + player.scale[1] : obsticles.pos[i][1] - player.scale[1]);
                player.pos[2] = (player.pos[2] > obsticles.pos[i][2] ? obsticles.pos[i][2] + player.scale[2] : obsticles.pos[i][2] - player.scale[2]);
            }
        }

    } // end of do
        while( glfwWindowShouldClose(win()) == 0 || camera.active);

    return deaths;
}


int main(int argc, char *argv[]) {
    // it let's you set text as seed
    // for some reason then 'w' char is in te arg it ignores the rest of the seed
    long int SEED;
    if(argc > 1){ 
        std::istringstream ss(argv[1]);
        if (!(ss >> SEED) || !ss.eof()) {
            SEED = std::strtol(argv[1], nullptr, 32);
            Misc::rand::setSeed(SEED);
        }
        else Misc::rand::setSeed(SEED);
    }
    std::cout << "Current seed: " << SEED << "\n";

    // convert arguntent for grid_size
    int grid_size = 10;
    if(argc > 2){ 
        std::istringstream ss(argv[2]);
        if (!(ss >> grid_size)) {
            std::cerr << "Invalid number: " << argv[2] << '\n';
        } else if (!ss.eof()) {
            std::cerr << "Trailing characters after number: " << argv[2] << '\n';
        }
    }

    MyWin win;
    win.Init(800,800,"Labirynt 3D - 309039",0,33);

    time_t start = time(0);

    int deaths =    win.MainLoop(grid_size);

    int seconds_since_start = difftime( time(0), start);
    std::cout<< "Time played: "  << seconds_since_start << "[s]\n" 
            << "Deaths count: " << deaths<<std::endl;

    return 0;
}