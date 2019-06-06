#ifndef INSTALLATION_H
#define INSTALLATION_H

class TypeInstallation{
    private:
	int id;
    int coeff;
    int portee;
    int price;

    public:
    TypeInstallation(int id, int coeff, int portee, int price);
    ~TypeInstallation();
    int getPrice();
    int getId();
    int getPortee();
    float getCoeff();
};

class Installation
{
    private:
    float x;
    float y;
    TypeInstallation type;
/*
    int coeff;
    int portee;
    int prix;*/

    public:
    Installation(TypeInstallation type); // Constructeur
    ~Installation(); // Destructeur

    float getPositionX();
    float getPositionY();
    int getType();

    void setPosition(float x, float y);

    void printPosition();
    void printInfos();
    bool isInShootingRange(float x, float y);
};

#endif // INSTALLATION_H
