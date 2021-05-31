#ifndef BACKGROUND_INCLUDED
#define BACKGROUND_INCLUDED

#include <AGL/AGL3Window.hpp>
#include <AGL/AGL3Drawable.hpp>
#include <Main/Misc.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

class Background : public AGLDrawable {
    GLdouble xs[3];
    GLdouble ys[3];
    GLdouble alphas[3];

    void move();

public:
   Background();
   void setShaders();
   void draw();

};
#endif