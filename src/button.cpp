#include "button.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;


Button::Button(float x, float y, float width, float height) : x(x), y(y), width(width), height(height)
{
    cout << "Create new button with x=" << x << " and y="<< y << endl;
}

Button::~Button(){};

bool Button::isHovered(float x, float y){
	return (x<=this->x) && (x>=this->x+this->width) && (y<=this->y) && (y>=this->y+this->height);
}
float Button::getX(){
	return this->x;
}
float Button::getY(){
	return this->y;
}
float Button::getWidth(){
	return this->width;
}
float Button::getHeight(){
	return this->height;
}