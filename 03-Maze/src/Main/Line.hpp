#ifndef LINE_INCLUDED
#define LINE_INCLUDED

#include <AGL/AGL3Window.hpp>
#include <AGL/AGL3Drawable.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

class Line : public AGLDrawable {
public:
    double_t length;
    double_t scale;
    double_t x;
    double_t y;
    Line(double_t X, double_t Y, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    Line(double_t X, double_t Y, GLfloat r, GLfloat g, GLfloat b);
    Line(double_t X, double_t Y, GLfloat c, GLfloat a);
    Line(double_t X, double_t Y, GLfloat c);
    Line(double_t X, double_t Y);
    void setRotation(double_t);
    void addRotation(double_t);
    const double_t getRotation() const;
    void setShaders();
    void draw();
    void setColor(float r, float g, float b, float a);
    void setColorHead(float r, float g, float b, float a);
    void setColorTail(float r, float g, float b, float a);
private:
    double_t rotation;
    GLfloat circle_color_0[4];
    GLfloat circle_color_1[4];
};
#endif