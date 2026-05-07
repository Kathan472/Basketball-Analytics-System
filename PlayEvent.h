// ============================================================
// PlayEvent.h
// Represents a single play-by-play event in the simulation.
// play_type values: "2PT", "3PT", "FT", "REBOUND", "FOUL"
// outcome values:   "MADE", "MISS", "OFF", "DEF", "SHOOTING"
// FOUL events reference the fouled shooter to support
// distance-based foul analysis.
// ============================================================
#ifndef PLAYEVENT_H
#define PLAYEVENT_H

#include <string>
using namespace std;

class PlayEvent {
private:
    int    play_id;
    int    player_id;
    string play_type; // "2PT", "3PT", "FT", "REBOUND", "FOUL"
    string outcome; // "MADE","MISS","OFF","DEF","SHOOTING"
    int    points; // raw points field from file (0 for misses/rebounds/fouls)
    double game_time; // continuous seconds from game start
    string real_clock; // display clock, e.g. "11:45"

public:
    // --- Constructors ---
    PlayEvent();
    PlayEvent(int pid, int player, const string& type,
              const string& outcome, int pts,
              double gtime, const string& clock);

    // --- Getters ---
    int    getPlayID()    const;
    int    getPlayerID()  const;
    string getPlayType()  const;
    string getOutcome()   const;
    int    getPoints()    const;
    double getGameTime()  const;
    string getRealClock() const;

    // --- Setters ---
    void setPlayID(int id);
    void setPlayerID(int id);
    void setPlayType(const string& type);
    void setOutcome(const string& outcome);
    void setPoints(int pts);
    void setGameTime(double t);
    void setRealClock(const string& clock);

    // --- Display ---
    void display() const;
};

#endif