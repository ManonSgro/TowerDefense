#include "player.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;


Player::Player(int pv) : pv(pv), money(100)
{
    cout << "Create new player with : \npv=" << pv << endl; 
}

Player::~Player(){};

bool Player::isAlive(){
    if(this->pv > 0){
        cout << "Player is alive." << endl;
    }else{
        cout << "Player is dead." << endl;
    }
    return this->pv > 0;
}

int Player::getPV(){
    return this->pv;
}

void Player::setPV(int pv){
    this->pv = pv;
}

int Player::getMoney(){
    return this->money;
}

void Player::setMoney(int money){
    this->money = money;
}