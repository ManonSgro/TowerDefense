#ifndef BUTTON_H
#define BUTTON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Button
{
    private:
	int x;
	int y;
	int width;
	int height;

    public:
    Button(int x, int y, int width, int height); // Constructeur
    ~Button(); // Destructeur
    bool isHovered(int x, int y);
    int getX();
    int getY();
    int getWidth();
    int getHeight();
};

#endif // BUTTON_H
