#ifndef BUTTON_H
#define BUTTON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Button
{
    private:
	float x;
	float y;
	float width;
	float height;

    public:
    Button(float x, float y, float width, float height); // Constructeur
    ~Button(); // Destructeur
    bool isHovered(float x, float y);
    float getX();
    float getY();
    float getWidth();
    float getHeight();
};

#endif // BUTTON_H
