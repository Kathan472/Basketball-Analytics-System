// ============================================================
// Game.h//
// Central analytics engine for basketball simulation,
// event processing, possession tracking, and reporting.
//
// File loading, analytics, and simulation systems
// Distance scoring breakdown
// Foul and free-throw analysis
// Recursive possession and team scoring reports
// ============================================================
#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "PlayEvent.h"
#include "TrackingPoint.h"
#include "GameEvent.h"

#include <string>
using namespace std;

// ── Possession linked list node ────────
struct PossessionNode {
    int    team;            // 1 or 2
    double start_time;      // game_time of first play in possession
    double end_time;        // game_time of last play in possession
    int    points;          // points scored during this possession
    int    play_count;      // number of play records in this possession
    PossessionNode* next;   // next possession in the chain

    PossessionNode(int t, double start)
        : team(t), start_time(start), end_time(start),
          points(0), play_count(0), next(nullptr) {}
};

// ── PlayLog: dynamic resizing array ─────────────
struct PlayLog {
    PlayEvent** plays;  // Dynamically resized array storing play pointers
    int count;
    int capacity;

    PlayLog();
    ~PlayLog();
    void add(PlayEvent* p);     // doubles capacity when full
    void print(int maxEntries) const;
};

class Game {
private:
    // raw data arrays ──────────────────────────────────
    Player* players; // static array, heap allocated
    int player_count;
    PlayEvent* play_events; // static array, heap allocated
    int play_count;
    TrackingPoint* tracking_points; // static array, heap allocated
    int tracking_count;

    // ── per-player dynamic play history ─────────
    PlayEvent*** playerPlays; // playerPlays[i] = array of pointers for player i+1
    int*         playerPlayCounts;
    bool         historiesBuilt;

    // ── possession linked list ──────────────────
    PossessionNode* possessionHead;
    bool            possessionBuilt;

    // ── polymorphic event array ─────────────────
    GameEvent** gameEvents; // array of GameEvent* (ShotEvent/FoulEvent/ReboundEvent)
    int         gameEventCount;
    int         gameEventCapacity;
    bool        simulationRun;

    // ── game scores ─────────────────────────────
    int team1Score;
    int team2Score;

    // ── Helper methods ──────────────────────────────────────

    // Find the tracking point closest in game_time for a given player
    TrackingPoint* getTrackingAt(int playerID, double gameTime) const;

    // Distance from (x,y) to the correct basket for a team
    double distanceToBasket(int teamID, double x, double y) const;

    // Build the polymorphic GameEvent array from play_events
    // Called once by simulateGame(); safe to call again
    void buildGameEvents();

    // Free all GameEvent objects and reset the array
    void clearGameEvents();

    // ── recursive helpers ───────────────────────
    // Returns sum of points for all possessions from node onward
    int recursivePossessionScore(PossessionNode* node) const;

    // Returns sum of points for team's possessions from node onward
    int recursiveTeamScore(PossessionNode* node, int team) const;

    // Returns pointer to the longest possession from node onward
    PossessionNode* recursiveLongestPossession(PossessionNode* node, PossessionNode* currentLongest) const;

    // ── PlayLog ──────────────────────────────────
    PlayLog playLog;

public:
    Game();
    ~Game();

    // ── File I/O ────────────────────────────────
    void readPlayerFile(const string& filename);
    void readPlayFile(const string& filename);
    void readTrackingFile(const string& filename);
    void printLoadSummary() const;

    // ── Data access (Options 1-8) ───────────────
    void displayAllPlayers()                      const;  // Option 1
    void getPlayerByID(int id)                    const;  // Option 2
    void getPlayByIndex(int index)                const;  // Option 3
    void getTrackingByIndex(int index)            const;  // Option 4
    void countPlays()                             const;  // Option 5
    void countTracking()                          const;  // Option 6
    void findPlayerByName(const string& name)     const;  // Option 7
    void displayLoadSummary()                     const;  // Option 8

    // ── Calculations (Options 9-12) ─────────────
    void getPlayerShotCount(int playerID)         const;  // Option 9
    void getPlayerTotalPoints(int playerID)       const;  // Option 10
    void getPlayerShootingPct(int playerID)       const;  // Option 11
    void getTeamTotalPoints(int teamID)           const;  // Option 12

    // ── Dynamic memory (Options 13-16) ──────────
    void testPlayLog(); // Option 13
    void buildPlayerPlayHistories(); // Option 14
    void displayPlayerHistory(int playerID)        const; // Option 15
    void analyzeShotLocations(int playerID)         const; // Option 16

    // ── Linked list (Options 17-20) ─────────────
    void buildPossessionChain(); // Option 17
    void displayPossessionChain()                 const;  // Option 18
    void analyzePossessionEfficiency()            const;  // Option 19
    void findLongestPossession()                  const;  // Option 20

    // ── Game simulation (Options 21-22) ─────────
    void simulateGame(); // Option 21
    void displayGameLog()                         const;  // Option 22

    // ── Distance breakdown (Option 23) ──────────
    void displayDistanceBreakdown()               const;  // Option 23

    // ── Foul summary (Option 24) ────────────────
    void displayFoulSummary()                     const;  // Option 24

    // ── Recursive reports (Options 25-26) ───────
    void recursivePossessionReport()              const;  // Option 25
    void recursiveTeamReport()                    const;  // Option 26
};

#endif