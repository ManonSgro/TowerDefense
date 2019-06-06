#include "button.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;


Button::Button(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
{
    cout << "Create new button with x=" << x << " and y="<< y << endl;
}

Button::~Button(){};

bool Button::isHovered(int x, int y){
	return (x>=this->x) && (x<=this->x+this->width) && (y>=this->y) && (y<=this->y+this->height);
}
int Button::getX(){
	return this->x;
}
int Button::getY(){
	return this->y;
}
int Button::getWidth(){
	return this->width;
}
int Button::getHeight(){
	return this->height;
}