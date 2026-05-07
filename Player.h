// ============================================================
// Player.h
// Stores player metadata used throughout the simulation engine.
// ============================================================
#ifndef PLAYER_H
#define PLAYER_H
#include <string>
using namespace std;

class Player {
private:
    int    player_id;
    string name;
    string position;
    int    jersey;
    int    team; // derived: IDs 1-5 = Team 1, IDs 6-10 = Team 2

public:
    // --- Constructors ---
    Player();
    Player(int id, const string& name, const string& pos, int jersey);

    // --- Getters ---
    int    getPlayerID()  const;
    string getName()      const;
    string getPosition()  const;
    int    getJersey()    const;
    int    getTeam()      const;

    // --- Setters ---
    void setPlayerID(int id);
    void setName(const string& name);
    void setPosition(const string& pos);
    void setJersey(int jersey);
    void setTeam(int team);

    // --- Display ---
    void display() const;
};

#endif