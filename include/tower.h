#ifndef TOWER_H
#define TOWER_H

class TypeTower
{
    private:
    int id;
    float coeffpuissance;
    float coeffportee;
    float coeffcadence;
    int price;

    public:
    TypeTower(int id, float coeffpuissance, float coeffportee, float coeffcadence, int price); // Constructeur
    ~TypeTower(); // Destructeur
    int getId();
    float getCoeffpuissance();
    float getCoeffportee();
    float getCoeffcadence();
    int getPrice();
    void printInfos();
};

class Bullet{
    private:
    float x;
    float y;
    float targetX;
    float targetY;

    public:
    Bullet(float x, float y,float targetX, float targetY);
    ~Bullet();
    bool moveToTarget();
    float getPositionX();
    float getPositionY();
    float getPositionTargetX();
    float getPositionTargetY();
};

class Tower
{
    private:
	int id;
    float x;
    float y;

    int puissance;
    int portee;
    int cadence;
    int conso;
    int prix;
    TypeTower type;
    int counter;

    public:
    Tower(int id, int puissance, int portee, int cadence, int conso, int prix, TypeTower type); // Constructeur
    ~Tower(); // Destructeur

    float getPositionX();
    float getPositionY();
    int getType();
    int getPuissance();
    int getCoeffPuissance();
    int getCounter();
    int getCadence();
    int getPortee();
    int getCoeffCadence();

    void setPosition(float x, float y);
    void setCounter(int counter);
    void setPuissance(int puissance);
    void setCadence(int cadence);
    void setPortee(int portee);

    void printPosition();
    void printInfos();
    bool isInShootingRange(float x, float y);
};

#endif // TOWER_H
