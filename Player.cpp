// ============================================================
// Player.cpp
// ============================================================
#include "Player.h"
#include <iostream>
using namespace std;

// ── Constructors ─────────────────────────────────────────────

// Initialize player with default values
Player::Player() {
    player_id = 0;
    name = "";
    position = "";
    jersey = 0;
    team = 0;
}

// Parameterized constructor
// Team assignment is derived from player ID ranges
// team = (id <= 5) ? 1 : 2
Player::Player(int id, const string& n, const string& pos, int j) {
    player_id = id;
    name = n;
    position = pos;
    jersey = j;
    team = (id <= 5) ? 1 : 2;
}

// ── Getters ──────────────────────────────────────────────────

int Player::getPlayerID() const {
    return player_id;
}

string Player::getName() const {
    return name;
}

string Player::getPosition() const {
    return position;
}

int Player::getJersey() const {
    return jersey;
}

int Player::getTeam() const {
    return team;
}

// ── Setters ──────────────────────────────────────────────────

void Player::setPlayerID(int id) {
    player_id = id;
    team = (id <= 5) ? 1 : 2;
}

void Player::setName(const string& n) {
    name = n;
}

void Player::setPosition(const string& p) {
    position = p;
}

void Player::setJersey(int j) {
    jersey = j;
}

void Player::setTeam(int t) {
    team = t;
}

// ── Display ──────────────────────────────────────────────────

// Print a one-line player summary.
// Expected format:
// ID: 1, Name: Sarah Johnson, Position: Guard, Jersey: 12, Team: 1
void Player::display() const {
    cout << "ID: " << player_id << ", Name: " << name << ", Position: " << position << ", Jersey: " << jersey << ", Team: " << team << endl;
}