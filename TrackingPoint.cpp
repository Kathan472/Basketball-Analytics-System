// ============================================================
// TrackingPoint.cpp
//
// Player movement tracking implementation.
//   Team 1 shoots at basket (0, 0)
//   Team 2 shoots at basket (94, 0)
// ============================================================
#include "TrackingPoint.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ── Constructors ─────────────────────────────────────────────
TrackingPoint::TrackingPoint() {
    tracking_id = 0;
    player_id = 0;
    x = 0.0;
    y = 0.0;
    game_time = 0.0;
}

TrackingPoint::TrackingPoint(int tid, int pid, double px, double py, double gt) {
    tracking_id = tid;
    player_id = pid;
    x = px;
    y = py;
    game_time = gt;
}

// ── Getters ──────────────────────────────────────────────────

int TrackingPoint::getTrackingID() const { 
    return tracking_id;  
}
int TrackingPoint::getPlayerID()   const { 
    return player_id;   
}
double TrackingPoint::getX()  const { 
    return x; 
}
double TrackingPoint::getY() const { 
    return y; 
}
double TrackingPoint::getGameTime() const { 
    return game_time; 
}

// ── Setters ──────────────────────────────────────────────────

void TrackingPoint::setTrackingID(int id) { 
    tracking_id = id; 
}
void TrackingPoint::setPlayerID(int id) { 
    player_id = id; 
}
void TrackingPoint::setX(double px) { 
    x = px; 
}
void TrackingPoint::setY(double py) { 
    y = py; 
}
void TrackingPoint::setGameTime(double t) { 
    game_time = t; 
}

// ── Display ──────────────────────────────────────────────────

// Print a one-line tracking point summary.
// Expected format:
//   Player 1 at 0.5s: (4.2, 2.1)
void TrackingPoint::display() const {
    // 5 getters: return the value of each private member variable
    cout << "Player " << player_id << " at " << fixed << setprecision(1) << game_time << "s: (" << x << ", " << y << ")" << endl;
}