#include "installation.h"
#include <iostream>
#include <cstring>
#include <cmath>
using namespace std;

TypeInstallation::TypeInstallation(int id, int coeff, int portee, int price) : id(id), coeff(coeff), portee(portee), price(price)
{
    cout << "Create new installation type with : id="<< id << " coeff=" << coeff << "\nportee=" << portee <<"\nprix=" << price << endl; 
}

TypeInstallation::~TypeInstallation(){};

int TypeInstallation::getPrice(){
    return this->price;
}

int TypeInstallation::getId(){
    return this->id;
}

float TypeInstallation::getCoeff(){
    return this->coeff;
}

int TypeInstallation::getPortee(){
    return this->portee;
}

Installation::Installation(TypeInstallation myType) : x(0.0), y(0.0), type(myType)
{
    cout << "Create new installation with : \ntype=" << this->type.getId() << endl; 
}

Installation::~Installation(){};


float Installation::getPositionX(){
    return this->x;
}

float Installation::getPositionY(){
    return this->y;
}

int Installation::getType(){
    return this->type.getId();
}

void Installation::setPosition(float x, float y){
    this->x = x;
    this->y = y;
}

void Installation::printPosition(){
    cout << "Position of installation : x=" << this->x << " y=" << this->y << endl;
}

void Installation::printInfos(){
    //cout << "Installation with : \nid=" << this->id << endl; 
}

bool Installation::isInShootingRange(float x, float y){
    int distance = sqrt(pow(this->x - x, 2) + pow(this->y - y, 2));
    //return (x<(this->x+this->portee) && x>(this->x-this->portee) && y<(this->y+this->portee) && y>(this->y-this->portee));
    return (distance<this->type.getPortee());
}