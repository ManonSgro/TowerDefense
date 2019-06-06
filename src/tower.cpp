#include "tower.h"
#include <iostream>
#include <cstring>
#include <cmath>
using namespace std;



TypeTower::TypeTower(int id, float coeffpuissance, float coeffportee, float coeffcadence, int price) : id(id), coeffpuissance(coeffpuissance), coeffportee(coeffportee), coeffcadence(coeffcadence), price(price)
{
    cout << "Create new type of tower with id=" << id << " and coeffpuissance="<< coeffpuissance << " and coeffportee="<< coeffportee << " and coeffcadence="<< coeffcadence << endl;
}

int TypeTower::getId(){
    return this->id;
}

float TypeTower::getCoeffpuissance(){
    return this->coeffpuissance;
}

float TypeTower::getCoeffportee(){
    return this->coeffportee;
}

float TypeTower::getCoeffcadence(){
    return this->coeffcadence;
}

int TypeTower::getPrice(){
    return this->price;
}

void TypeTower::printInfos(){
    cout << "Type of tower with id=" << this->id << " and coeffpuissance="<< this->coeffpuissance << " and coeffportee="<< this->coeffportee << " and coeffcadence="<< this->coeffcadence << endl;
}

TypeTower::~TypeTower(){};




Tower::Tower(int id, int puissance, int portee, int cadence, int conso, int prix, TypeTower type) : id(id), puissance(puissance), portee(portee), cadence(cadence), conso(conso),prix(prix), type(type), x(0.0), y(0.0), counter(0)
{
    cout << "Create new tower with : \nid=" << id << "\npuissance=" << puissance <<"\nportee="<< portee << "\ncadence=" << cadence << "\nconso=" << conso << "\nprix=" << prix << "\nType=" << type.getId() << endl; 
}

Tower::~Tower(){};


float Tower::getPositionX(){
    return this->x;
}

float Tower::getPositionY(){
    return this->y;
}

void Tower::setPosition(float x, float y){
    this->x = x;
    this->y = y;
}

void Tower::printPosition(){
    cout << "Position of tower : x=" << this->x << " y=" << this->y << endl;
}

void Tower::printInfos(){
    cout << "Tower with : \nid=" << this->id << "\npuissance=" << this->puissance <<"\nportee="<< this->portee << "\ncadence=" << this->cadence << "\nconso=" << this->conso << "\nprix=" << this->prix << "\nType=" << type.getId() << endl; 
}

int Tower::getType(){
    return this->type.getId();
}

int Tower::getCoeffPuissance(){
    return this->type.getCoeffpuissance();
}

int Tower::getPuissance(){
    return this->puissance;
}

int Tower::getCounter(){
    return this->counter;
}

int Tower::getCadence(){
    return this->cadence;
}

int Tower::getPortee(){
    return this->portee;
}

int Tower::getCoeffCadence(){
    return this->type.getCoeffcadence();
}

void Tower::setCounter(int counter){
    this->counter = counter;
}

void Tower::setPuissance(int puissance){
    this->puissance = puissance;
}

void Tower::setCadence(int cadence){
    this->cadence = cadence;
}

void Tower::setPortee(int portee){
    this->portee = portee;
}

bool Tower::isInShootingRange(float x, float y){
    int distance = sqrt(pow(this->x - x, 2) + pow(this->y - y, 2));
    //return (x<(this->x+this->portee) && x>(this->x-this->portee) && y<(this->y+this->portee) && y>(this->y-this->portee));
    return (distance<this->portee);
}

Bullet::Bullet(float x, float y,float targetX, float targetY) : x(x), y(y), targetX(targetX), targetY(targetY)
{
    //cout << "Create new bullet with x=" << x << " and y="<< y << endl;
}

bool Bullet::moveToTarget(){
    if(this->x >= this->targetX-0.03 && this->x <= this->targetX+0.03 && this->y <= this->targetY+0.03 && this->y >= this->targetY-0.03){
        return false;
    }else{
        float coef = (float)(this->y-this->targetY)/(float)(this->x-this->targetX);
        float cste = this->y-(coef*this->x);
        if(this->targetX>this->x){
            this->x+=0.03;
        }else if(this->targetX<this->x && this->targetY!=this->x){
            this->x-=0.03;
        }
        this->y = coef*(this->x)+cste;
        return true;
    }
}

float Bullet::getPositionX(){
    return this->x;
}

float Bullet::getPositionY(){
    return this->y;
}

float Bullet::getPositionTargetX(){
    return this->x;
}

float Bullet::getPositionTargetY(){
    return this->y;
}