#include "monster.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;


TypeMonster::TypeMonster(int id, int speed, Resistance resistance, int degats) : id(id), speed(speed), resistance(resistance), degats(degats)
{
    cout << "Create new type of monster with id=" << id << " and speed="<< speed << endl;
}

int TypeMonster::getId(){
    return this->id;
}

int TypeMonster::getSpeed(){
    return this->speed;
}

int TypeMonster::getDegats(){
    return this->degats;
}

void TypeMonster::printInfos(){
    cout << "Type of monster with id=" << this->id << " and speed="<< this->speed << endl;
}

TypeMonster::~TypeMonster(){};

TypeMonster defaultType(1,1, strong, 10);

Monster::Monster() : id(0), pv(100), wave(0), type(defaultType), x(0.0), y(0.0), idCurrentNode(0), typeCurrentNode(0), currentNode({0,0}), nextNode({0,0})
{
    //cout << "Create new monster with : \nid=" << id << "\npv=" << pv <<"\nwave_id=" << wave << "\nType=" << type.getId() << endl; 
}

Monster::Monster(int id, int wave, TypeMonster type) : id(id), pv(100), wave(wave), type(type), x(0.0), y(0.0), idCurrentNode(0), typeCurrentNode(0), currentNode({0,0}), nextNode({0,0})
{
    //cout << "Create new monster with : \nid=" << id << "\npv=" << pv << "\nwave_id=" << wave << "\nType=" << type.getId() << endl; 
}

Monster::~Monster(){};

void Monster::init(int id, int wave, TypeMonster type){
    this->id = id;
    this->pv = 100;
    this->wave = wave;
    this->type = type;
    this->x = 0.0;
    this->y = 0.0;
}

bool Monster::isAlive(){
    if(this->pv > 0){
        //cout << "Monster is alive." << endl;
    }else{
        //cout << "Monster is dead." << endl;
    }
    return this->pv > 0;
}

int Monster::getPV(){
    return this->pv;
}

int Monster::getSpeed(){
    return this->type.getSpeed();
}

int Monster::getIdType(){
    return this->type.getId();
}

float Monster::getPositionX(){
    return this->x;
}

float Monster::getPositionY(){
    return this->y;
}

int Monster::getDegats(){
    return this->type.getDegats();
}

int Monster::getTypeCurrentNode(){
    return this->typeCurrentNode;
}

int Monster::getIdCurrentNode(){
    return this->idCurrentNode;
}

int Monster::getCurrentNodeX(){
    return this->currentNode[0];
}

int Monster::getCurrentNodeY(){
    return this->currentNode[1];
}

int Monster::getIdNextNode(){
    return this->idNextNode;
}

int Monster::getNextNodeX(){
    return this->nextNode[0];
}

int Monster::getNextNodeY(){
    return this->nextNode[1];
}

void Monster::setPV(int pv){
    this->pv = pv;
}

void Monster::setPosition(float x, float y){
    this->x = x;
    this->y = y;
}

void Monster::setNextNode(int id, int x, int y){
    this->idNextNode = id;
    this->nextNode[0] = x;
    this->nextNode[1] = y;
}

void Monster::setCurrentNode(int id, int type, int x, int y){
    this->idCurrentNode = id;
    this->typeCurrentNode = type;
    this->currentNode[0] = x;
    this->currentNode[1] = y;
}

void Monster::printPosition(){
    cout << "Position of monster : x=" << this->x << " y=" << this->y << endl;
}

void Monster::printInfos(){
    cout << "Monster with : \nid=" << this->id << "\npv=" << this->pv <<"\nwave_id=" << this->wave << "\nType=" << type.getId() << endl; 
}

bool Monster::moveToNode(int x, int y){
	float coef = (float)(this->y-y)/(float)(this->x-x);
	float cste = this->y-(coef*this->x);
	float newX;
    int randNum = rand()%(this->getSpeed()+this->getSpeed() + 1) - this->getSpeed();
	if(x>this->x){
		newX = this->x+this->getSpeed();
	}else if(x<this->x && y!=this->x){
		newX = this->x-this->getSpeed();
	}else{
		newX = this->x;
	}
	float newY = coef*(newX)+cste;
	this->setPosition(newX+randNum, newY+randNum);
    //this->setPosition(newX, newY);
	if(this->x == x && this->y == y){ //collision
        return true;
    }
	return false;
}
/*
Wave::Wave(int id, Monster* monsters[]) : id(id), monsters(monsters)
{
    cout << "Create new wave with : \nid=" << id << "\nFirst monster=" << monsters[0]->getPV() << endl; 
}

Monster::~Wave(){};

void Wave::createWave(){
    this->id = id;
    this->
}*/