#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    private:
	int pv;
	int money;
    

    public:
    Player(int pv); // Constructeur
    ~Player(); // Destructeur

    bool isAlive();

    int getPV();
    int getMoney();

    void setPV(int pv);
    void setMoney(int money);
};

#endif // PLAYER_H