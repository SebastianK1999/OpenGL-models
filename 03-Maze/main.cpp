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



class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
        : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    virtual void KeyCB(int key, int scancode, int action, int mods);
    int MainLoop(long int grid_size);
    bool detectColision(const Line& player, const Line* other);
};
bool MyWin::detectColision(const Line& player, const Line* other_p){
   if(pow( player.x - other_p->x, 2) + pow( player.y - other_p->y, 2) < pow(player.scale + other_p->scale,2)){
      double_t tg_a1 = std::tan(player.getRotation());
      double_t tg_a2 = std::tan(other_p->getRotation());

      double_t x = (other_p->y - player.y + tg_a1 * player.x - tg_a2 * other_p->x) / (tg_a1 - tg_a2);
      double_t y = std::tan(player.getRotation()) * (x - player.x) + player.y;

      if(pow( player.x - x, 2) + pow( player.y - y, 2) <= player.scale * player.scale &&
         pow( x - other_p->x, 2) + pow( y - other_p->y, 2) <= other_p->scale * other_p->scale
         ) return true;
   }
   return false;
}


void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
       ; // do something
    }
    if (key == GLFW_KEY_HOME  && (action == GLFW_PRESS)) {
       ; // do something
    }
}

void reshape(GLFWwindow * a, int b,int c) {
         if(b < c){
            int rest = (c-b)/2;
            glViewport(0, rest, b, b);
         }
         else{
            int rest = (b-c)/2;
            glViewport(rest, 0, c, c);
         }
}


int MyWin::MainLoop(long int grid_size) {
   ViewportOne(0,0,wd,ht);
   glfwSetWindowSizeCallback(win(), reshape);

   int deaths = 0;

   long int grid_len = grid_size * grid_size - 1;

   Line* grid[grid_len];

   double_t scale = 1.0/grid_size; // śrdki w przedziale [-0,9, 0.9] dla grid_size == 10, dla innych grid_size przedział musi być inny żeby efekt był sensowny
   for(int i =0; i<=grid_len; i++){
      grid[i] = new Line(
         ((double)((grid_len-i)%grid_size) - (grid_size-1.0)/2) * scale * 2,
         ((double)((grid_len-i)/grid_size) - (grid_size-1.0)/2) * scale * 2,
         1
      );
      grid[i]->scale = scale;
      grid[i]->setRotation(Misc::rand::get().Decim(M_PI));
   }
   grid[0]->setColorTail(1,1,1,1);
   grid[0]->setColorHead(0,1,0,1);

   Line* bounds[4];
   bounds[0] = new Line( 1,  0, 1);
   bounds[1] = new Line( 0,  1, 1);
   bounds[2] = new Line(-1,  0, 1);
   bounds[3] = new Line( 0, -1, 1);
   bounds[0]->setRotation(M_PI/2);
   bounds[1]->setRotation(0);
   bounds[2]->setRotation(M_PI/2);
   bounds[3]->setRotation(0);


   Line player(
      -1.0 + scale, -1.0 + scale,
      1.0, 0.0, 1.0, 1.0
   );
   player.scale = scale;
   player.setColorTail(0.0,1.0,1.0,0.0);

   Background bg;

   
   glfwSetTime(0.0);
   do {
      glClear( GL_COLOR_BUFFER_BIT );

      AGLErrors("main-loopbegin");
      // =====================================================        Drawing
      bg.draw();
      player.draw();
      for(int i =0; i<grid_len; i++){
         grid[i]->draw();
      } 
      for(int i
       =0; i<4; i++){
         bounds[i]->draw();
      } 

      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win()); // =============================   Swap buffers
      glfwPollEvents();
      //glfwWaitEvents();   

      // Movement
      if (glfwGetKey(win(), GLFW_KEY_UP ) == GLFW_PRESS) {
         player.x += std::cos(player.getRotation()) * 0.25 * glfwGetTime();
         player.y += std::sin(player.getRotation()) * 0.25 * glfwGetTime();
      }
      if (glfwGetKey(win(), GLFW_KEY_DOWN ) == GLFW_PRESS) {
         player.x -= std::cos(player.getRotation()) * 0.25 * glfwGetTime();
         player.y -= std::sin(player.getRotation()) * 0.25 * glfwGetTime();
      }
      if (glfwGetKey(win(), GLFW_KEY_LEFT ) == GLFW_PRESS) {
         player.addRotation(1.3 * glfwGetTime());
      }
      if (glfwGetKey(win(), GLFW_KEY_RIGHT ) == GLFW_PRESS) {
         player.addRotation(-1.3 * glfwGetTime());
      }
      glfwSetTime(0.0);


      // Colisions
      if(detectColision(player, grid[0])){
         break;
      }
      
      for(int i = 1; i<grid_len; i++){
         if(detectColision(player, grid[i])){
            player.x = -1 + scale;
            player.y = -1 + scale;
            deaths++;
         }
      }
      for(int i = 1; i<4; i++){
         if(detectColision(player, bounds[i])){
            player.x = -1 + scale;
            player.y = -1 + scale;
            deaths++;
         }
      } 

   } // end of do
      while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
   
   for(int i =0; i<grid_len; i++){
      delete grid[i];
   }
   for(int i =0; i<4; i++){
      delete bounds[i];
   } 
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
   win.Init(800,800,"Labirynt - 309039",0,33);

   time_t start = time(0);

   int deaths =    win.MainLoop(grid_size);
   
   int seconds_since_start = difftime( time(0), start);
   std::cout<< "Time played: "  << seconds_since_start << "[s]\n" 
            << "Deaths count: " << deaths<<std::endl;

   return 0;
}