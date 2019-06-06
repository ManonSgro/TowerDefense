#ifndef MONSTER_H
#define MONSTER_H

typedef enum Resistance {strong=10, medium=20, weak=30};

class TypeMonster
{
    private:
	int id;
    int speed;
    Resistance resistance;
    int degats;

    public:
    TypeMonster(int id, int speed, Resistance resistance, int degats); // Constructeur
    ~TypeMonster(); // Destructeur
    int getId();
    int getSpeed();
    int getDegats();
    void printInfos();
};

class Monster
{
    private:
	int id;
	int pv;
	int wave;
    TypeMonster type;
    float x;
    float y;
    int idCurrentNode;
    int currentNode[2];
    int typeCurrentNode;
    int idNextNode;
    int nextNode[2];
    

    public:
    Monster(); //default constructor
    Monster(int id, int wave, TypeMonster type); // Constructeur
    ~Monster(); // Destructeur

    void init(int id, int wave, TypeMonster type);
    bool isAlive();
    bool moveToNode(int x, int y);
    //bool moveToNode();

    int getPV();
    int getSpeed();
    int getIdType();
    float getPositionX();
    float getPositionY();
    int getIdCurrentNode();
    int getTypeCurrentNode();
    int getCurrentNodeX();
    int getCurrentNodeY();
    int getIdNextNode();
    int getNextNodeX();
    int getNextNodeY();
    int getDegats();

    void setPV(int pv);
    void setPosition(float x, float y);
    void setNextNode(int id, int x, int y);
    void setCurrentNode(int id, int type, int x, int y);

    void printPosition();
    void printInfos();
};

/*class Wave
{
    private:
	int id;
    Monster* monsters[];

    public:
    Wave(int id, Monster* monsters[]); // Constructeur
    ~Wave(); // Destructeur

    bool createWave(int x, int y);
};*/

#endif // MONSTER_H
