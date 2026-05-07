// ============================================================
// TrackingPoint.h
// Represents a player position sample captured during gameplay.
// Court: 94 ft x 50 ft
//   Team 1 shoots at basket (0, 0)
//   Team 2 shoots at basket (94, 0)
// ============================================================
#ifndef TRACKINGPOINT_H
#define TRACKINGPOINT_H
#include <string>
using namespace std;

class TrackingPoint {
private:
    int    tracking_id;
    int    player_id;
    double x; // court x position (0-94 ft)
    double y; // court y position (0-50 ft)
    double game_time; // continuous seconds from game start
public:
    // --- Constructors ---
    TrackingPoint();
    TrackingPoint(int tid, int pid, double x, double y, double gtime);

    // --- Getters ---
    int    getTrackingID() const;
    int    getPlayerID()   const;
    double getX()          const;
    double getY()          const;
    double getGameTime()   const;
    // --- Setters ---
    void setTrackingID(int id);
    void setPlayerID(int id);
    void setX(double x);
    void setY(double y);
    void setGameTime(double t);

    // --- Display ---
    void display() const;
};

#endif