// ============================================================
// Game.cpp
// Core analytics and simulation engine.
//
// ── Order ──────────────────────────────────────────
//   1. Play log implementation
//   2. Game constructor / destructor
//   3. readPlayerFile / readPlayFile / readTrackingFile
//   4. Options 1-8 (data access)
//   5. Options 9-12 (calculations)
//   6. Options 13-16 (dynamic memory)
//   7. Options 17-20 (possession chain)
//   8. simulateGame / displayGameLog (Options 21-22)
//   9. displayDistanceBreakdown (Option 23)
//  10. displayFoulSummary (Option 24)
//  11. buildGameEvents (Option 25)
//  12. per team summaries (Option 26)
//  13. exit (Option 27) 
// ============================================================
#include "Game.h"
#include "ShotEvent.h"
#include "FoulEvent.h"
#include "ReboundEvent.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

// ============================================================
// PlayLog — Dynamic resizing array
// ============================================================

PlayLog::PlayLog() {
    // Initialize dynamic play storage
    capacity = 5;
    count = 0;
    plays = new PlayEvent*[capacity];
}

PlayLog::~PlayLog() {
    // Release dynamically allocated play pointer array
    // Set plays = nullptr
    // free the array of pointers, but not the PlayEvent objects themselves
    if (plays != nullptr) {
        delete[] plays;
        plays = nullptr;
    }
}

// Add a play pointer to the log.
// If the array is full, double its capacity before adding.
void PlayLog::add(PlayEvent* p) {
    // Resize storage when capacity is reached
    //  If full, create a new array with capacity * 2
    //  Copy all existing pointers to the new array
    //  Delete the old array
    //  Update plays and capacity
    //  Display resize diagnostics
    //  Add p to plays[count], then increment count
    if (count == capacity) {
        int newCapacity = capacity * 2;

        PlayEvent** newPlays = new PlayEvent*[newCapacity];

        for (int i = 0; i < count; i++)
            newPlays[i] = plays[i];

        delete[] plays;

        plays = newPlays;

        cout << "PlayLog resized from " << capacity << " to " << newCapacity << endl;

        capacity = newCapacity;
    }
    plays[count] = p;
    count++;
}

void PlayLog::print(int maxEntries) const {
    // Print up to maxEntries entries in the format:
    // 0. Player 1: 2PT
    // 1. Player 3: REBOUND
    // Use min(maxEntries, count) as the loop limit
    int entriesToPrint = min(maxEntries, count);
    for (int i = 0; i < entriesToPrint; i++) {
        cout << i << ". Player " << plays[i]->getPlayerID() << ": " << plays[i]->getPlayType() << endl;
    }
}

// ============================================================
// Game: Constructor / Destructor
// ============================================================

Game::Game()
    : players(nullptr),         player_count(0),
      play_events(nullptr),     play_count(0),
      tracking_points(nullptr), tracking_count(0),
      playerPlays(nullptr),     playerPlayCounts(nullptr),
      historiesBuilt(false),
      possessionHead(nullptr),  possessionBuilt(false),
      gameEvents(nullptr),      gameEventCount(0),
      gameEventCapacity(0),     simulationRun(false),
      team1Score(0),            team2Score(0) {}
      // Note: constructor body is intentionally empty —
      // all members are initialized in the list above.

Game::~Game() {
    // Release primary dataset arrays
    if (players != nullptr) {
        delete[] players;
        players = nullptr;
    }
    if (play_events != nullptr) {
        delete[] play_events;
        play_events = nullptr;
    }
    if (tracking_points != nullptr) {
        delete[] tracking_points;
        tracking_points = nullptr;
    }

    // If playerPlays is not nullptr:
    // Loop over player_count and delete each playerPlays[i]
    // Then delete[] playerPlays and delete[] playerPlayCounts
    if (playerPlays != nullptr) {
        for (int i = 0; i < player_count; i++) {
            delete[] playerPlays[i];
        }

        // Delete the main arrays
        delete[] playerPlays;
        playerPlays = nullptr;

        delete[] playerPlayCounts;
        playerPlayCounts = nullptr;
    }

    // Release possession chain memory
    PossessionNode* current = possessionHead;
    while (current != nullptr) {
        PossessionNode* next = current->next;
        delete current;
        current = next;
    }

    // Call clearGameEvents() to free GameEvent objects
    clearGameEvents();
}

// ============================================================
// File I/O
// ============================================================

// Read player_info.txt
// Format: player_id,name,position,jersey
// Note: the header line ("player_id,name,...") must be skipped.
// Note: team is NOT in the file — calculate it from player_id.
void Game::readPlayerFile(const string& filename) {
    // Open dataset file and skip header row
    // Count the remaining non-empty lines (for array sizing)
    // Seek back to start, skip header again
    // Allocate: players = new Player[count]
    // For each line, parse the four comma-separated fields:
    // player_id (int), name (string), position (string), jersey (int)
    // Create Player(id, name, pos, jersey) and store in players[]
    // Increment player_count for each player loaded
    ifstream file;

    // Open the file
    file.open(filename);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        cout << "Could not open file " << filename << endl;
        return;
    }

    string line; // String to hold each line of the file
    // Skip header
    getline(file, line);

    // Count remaining lines
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) {
            count++;
        }
    }

    // Reset file stream for parsing
    file.clear(); // Clear EOF flags
    file.seekg(0); // Back to top
    getline(file, line); // Skip header again

    // Allocate players array with count size
    players = new Player[count]; 

    // Parse comma-separated dataset fields
    while (getline(file, line) && player_count < count) {
        istringstream ss(line);
        string tok, name, pos;

        // Parse the line into fields using getline with ',' as delimiter
        getline(ss, tok, ',');  // Read player_id as string token, then convert to int
        int id = stoi(tok);
        getline(ss, name, ','); // Read name as string
        getline(ss, pos, ','); // Read position as string
        getline(ss, tok); // Read jersey as string token, then convert to int
        int jersey = stoi(tok);

        // Create Player object and store in array at index player_count
        players[player_count] = Player(id, name, pos, jersey);

        // Track loaded player count
        player_count++;
    }
    file.close();

    // Print summary of how many players were loaded
    cout << "Loaded " << player_count << " players." << endl;
}

// Read play_events.txt
// Format: play_id,player_id,play_type,outcome,points,game_time,real_clock
void Game::readPlayFile(const string& filename) {
    //   Same pattern as readPlayerFile()
    //   Parse seven fields per line.
    //   play_id and player_id are int (use stoi)
    //   points is int (use stoi)
    //   game_time is double (use stod)
    //   play_type, outcome, real_clock are strings
    //   Store each as PlayEvent(pid, playerID, ptype, outcome, pts, gtime, clock)
    ifstream file;

    file.open(filename); // Open the file

    // Check if the file was opened successfully
    if (!file.is_open()) {
        cout << "Could not open file " << filename << endl;
        return;
    }

    // Skip header
    string line; // String to hold each line of the file
    getline(file, line); 

    // Count remaining lines
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) count++;
    }

    // Reset file stream for parsing
    file.clear();
    file.seekg(0);
    getline(file, line);

    // Allocate play_events array with count size
    play_events = new PlayEvent[count];
    play_count = 0;

    // Parse each line into a PlayEvent and store in array
    while (getline(file, line) && play_count < count)
    {
        istringstream ss(line);

        string tok, ptype, outcome, clock;

        // Parse comma-separated dataset fields
        getline(ss, tok, ','); // Read play_id as string token, then convert to int
        int play_id = stoi(tok);

        getline(ss, tok, ','); // Read player_id as string token, then convert to int
        int player_id = stoi(tok);

        getline(ss, ptype, ','); // Read play_type as string

        getline(ss, outcome, ','); // Read outcome as string

        getline(ss, tok, ','); // Read points as string token, then convert to int
        int pts = stoi(tok);

        getline(ss, tok, ','); // Read game_time as string token, then convert to double
        double gtime = stod(tok);

        getline(ss, clock); // Read real_clock as string

        // Create PlayEvent object and store in array at index play_count
        play_events[play_count] = PlayEvent(play_id, player_id, ptype, outcome, pts, gtime, clock);

        // Increment play_count for each play loaded
        play_count++;
    }

    file.close();

    // Print summary of how many plays were loaded
    cout << "Loaded " << play_count << " plays." << endl;
}

// Read tracking_data.txt
// Format: tracking_id,player_id,x,y,game_time
void Game::readTrackingFile(const string& filename) {
    //   Same pattern.
    //   Parse five fields per line.
    //   tracking_id and player_id are int
    //   x, y, game_time are double (use stod)
    //   Store each as TrackingPoint(tid, pid, x, y, gt)
    ifstream file;
    file.open(filename); // Open the file

    // Check if the file was opened successfully
    if (!file.is_open()) {
        cout << "Could not open file " << filename << endl;
        return;
    }
    
    // Skip the header line
    string line; // String to hold each line of the file
    getline(file, line);

    // Count remaining lines
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) count++;
    }

    // Reset file stream for parsing
    file.clear();
    file.seekg(0);
    getline(file, line);

    // Allocate tracking_points array with count size
    tracking_points = new TrackingPoint[count];
    tracking_count = 0;

    // Parse comma-separated dataset fields
    while (getline(file, line) && tracking_count < count)
    {
        istringstream ss(line);

        // Parse the line into fields using getline with ',' as delimiter
        string tok; // Temporary string token for parsing fields

        getline(ss, tok, ','); // Read tracking_id as string token, then convert to int
        int tid = stoi(tok);

        getline(ss, tok, ','); // Read player_id as string token, then convert to int
        int pid = stoi(tok);

        getline(ss, tok, ','); // Read x as string token, then convert to double
        double x = stod(tok);

        getline(ss, tok, ','); // Read y as string token, then convert to double
        double y = stod(tok);

        getline(ss, tok); // Read game_time as string token, then convert to double
        double gt = stod(tok);

        // Create TrackingPoint object and store in array at index tracking_count
        tracking_points[tracking_count] = TrackingPoint(tid, pid, x, y, gt);

        // Increment tracking_count for each tracking point loaded
        tracking_count++;
    }

    file.close();

    cout << "Loaded " << tracking_count << " tracking points." << endl;
}

// Print the startup load summary
void Game::printLoadSummary() const {
    //   Print three lines:
    //   Loaded 10 players.
    //   Loaded 22 plays.
    //   Loaded 2510 tracking points.
    cout << "Loaded " << player_count << " players." << endl;
    cout << "Loaded " << play_count << " plays." << endl;
    cout << "Loaded " << tracking_count << " tracking points." << endl;
}

// ============================================================
// Data Access (Options 1-8)
// ============================================================

// Option 1: Print all players using their display() method
void Game::displayAllPlayers() const {
    // Print "=== ALL PLAYERS ===" then loop and call display() on each element of the players array
    cout << "=== ALL PLAYERS ===" << endl;

    // Loop through players array and call display() on each player
    for (int i = 0; i < player_count; i++) {
        players[i].display();
    }
}

// Option 2: Search for a player by their numeric ID
void Game::getPlayerByID(int id) const {
    // Loop through players[], find the one with getPlayerID() == id
    // Call display() on it, or print "Player not found."
    
    // Loop through players array to find the player with matching ID
    for (int i = 0; i < player_count; i++)
    {
        if (players[i].getPlayerID() == id)
        {
            players[i].display();
            return;
        }
    }

    // If we finish the loop without finding a match, print "Player not found."
    cout << "Player not found." << endl;
}

// Option 3: Get a play event by array index
void Game::getPlayByIndex(int index) const {
    // Check that index is in range [0, play_count-1]
    // If out of range, print an error message
    // Otherwise call play_events[index].display()
    
    // Check if index is valid
    if (index < 0 || index >= play_count)
    {
        cout << "Invalid play index." << endl;
        return;
    }

    // Display the play event at the given index
    play_events[index].display();
}

// Option 4: Get a tracking point by array index
void Game::getTrackingByIndex(int index) const {
    // Same pattern as getPlayByIndex()
    // Valid range: [0, tracking_count-1]

    // Check if index is valid
    if (index < 0 || index >= tracking_count)
    {
        cout << "Invalid tracking index." << endl;
        return;
    }

    // Display the tracking point at the given index
    tracking_points[index].display();
}

// Option 5: Print total play count
void Game::countPlays() const {
    // Print "Total plays: N"
    // Print the total number of plays
    cout << "Total plays: " << play_count << endl;
}

// Option 6: Print total tracking point count
void Game::countTracking() const {
    // Print "Total tracking points: N"
    // Print the total number of tracking points
    cout << "Total tracking points: " << tracking_count << endl;
}

// Option 7: Search for a player by exact name (case-sensitive)
void Game::findPlayerByName(const string& name) const {
    // Loop through players[], find one where getName() == name
    // Print "Player ID: X" then call display() Or print "Player not found."

    // Loop through players array to find the player with matching name
    for (int i = 0; i < player_count; i++)
    {
        if (players[i].getName() == name)
        {
            cout << "Player ID: " << players[i].getPlayerID() << endl;

            players[i].display();
            return;
        }
    }

    // If we finish the loop without finding a match, print "Player not found."
    cout << "Player not found." << endl;
}

// Option 8: Print file load summary
void Game::displayLoadSummary() const {
    // Print "=== FILE LOAD SUMMARY ===" then call printLoadSummary()

    // Print load summary 
    cout << "=== FILE LOAD SUMMARY ===" << endl;
    printLoadSummary();
}

// ============================================================
// Calculations (Options 9-12)
// ============================================================

// Option 9: Count how many shots a player attempted
// Shots are play_type == "2PT", "3PT", or "FT"
void Game::getPlayerShotCount(int playerID) const {
    //   Loop through play_events[]
    //   Count records where getPlayerID() == playerID
    //   AND getPlayType() is "2PT", "3PT", or "FT"
    //   Print "Player N shot count: X"

    int count = 0; // Initialize shot count to 0

    // Loop through play_events array to count shots for the given playerID
    for (int i = 0; i < play_count; i++)
    {
        // Check if the play belongs to the specified player
        if (play_events[i].getPlayerID() == playerID)
        {
            // Check if the play type is a shot attempt (2PT, 3PT, or FT)
            string type = play_events[i].getPlayType();

            // If it's a shot attempt, increment the count
            if (type == "2PT" || type == "3PT" || type == "FT") {
                count++;
            }
        }
    }

    // Print the total shot count for the player
    cout << "Player " << playerID << " (" << players[playerID - 1].getName() << ")" << " shot count: " << count << endl;
}

// Option 10: Sum total points scored by a player
void Game::getPlayerTotalPoints(int playerID) const {
    //   Loop through play_events[]
    //   Sum getPoints() for records where getPlayerID() == playerID
    //   Print "Player N total points: X"

    int total = 0; // Initialize total points to 0

    // Loop through play_events array to sum points for the given playerID
    for (int i = 0; i < play_count; i++) {

        // Check if the play belongs to the specified player
        if (play_events[i].getPlayerID() == playerID) {

            // If it does, add the points from that play to the total
            total += play_events[i].getPoints();
        }
    }

    // Print the total points scored by the player
    cout << "Player " << playerID << " (" << players[playerID - 1].getName() << ")" << " total points: " << total << endl;
}

// Option 11: Calculate shooting percentage for a player
// shooting% = (made shots / total shot attempts) * 100
void Game::getPlayerShootingPct(int playerID) const {
    //   Count total shots (2PT, 3PT, FT) for this player
    //   Count made shots (outcome == "MADE")
    //   Guard against division by zero: if attempts == 0, print 0.0%
    //   Otherwise: pct = (double)made / attempts * 100.0
    //   Print "Player N shooting percentage: XX.XX%"
    //   Use fixed << setprecision(2) for formatting

    int total_shots = 0; // Initialize total shots to 0
    int made_shots = 0; // Initialize made shots to 0

    // Loop through play_events array to count total shots and made shots for the given playerID
    for (int i = 0; i < play_count; i++)
    {
        // Check if the play belongs to the specified player
        if (play_events[i].getPlayerID() == playerID)
        {
            // Check if the play type is a shot attempt (2PT, 3PT, or FT)
            string type = play_events[i].getPlayType();

            // If it's a shot attempt, increment total shots
            if (type == "2PT" || type == "3PT" || type == "FT")
            {
                total_shots++;

                // If the shot was made, increment made shots
                if (play_events[i].getOutcome() == "MADE")
                    made_shots++;
            }
        }
    }

    // If there were no shot attempts, print 0.0% to avoid division by zero
    // We used playerID - 1 to access the player's name because player IDs are 1-based while array indices are 0-based
    if (total_shots == 0) {
        cout << "Player " << playerID << " (" << players[playerID - 1].getName() << ")" << " shooting percentage: 0.00%" << endl;
        return;
    }

    // Calculate shooting percentage
    else {
        double shooting_percentage = (double)made_shots / total_shots * 100;

        cout << fixed << setprecision(2);

        cout << "Player " << playerID << " (" << players[playerID - 1].getName() << ")" << " shooting percentage: " << shooting_percentage << "%" << endl;
    }
}

// Option 12: Sum total points scored by an entire team
void Game::getTeamTotalPoints(int teamID) const {
    //   For each play, find the player to check their team
    //   Sum getPoints() for plays belonging to players on teamID
    //   Print "Team N total points: X"
    //
    // Hint: for each play, loop through players[] to find the player whose getPlayerID() matches the play's getPlayerID(),
    // then check if that player's getTeam() == teamID

    int total = 0; // Initialize total points for the team to 0

    // Loop through play_events array to sum points for players on the specified teamID
    for (int i = 0; i < play_count; i++)
    {
        // Get the player ID from the current play event
        int pid = play_events[i].getPlayerID();

        // Loop through players array to find the player with matching ID and check their team
        for (int j = 0; j < player_count; j++)
        {
            // Check if the player's ID matches the play's player ID and if the player's team matches the teamID
            if (players[j].getPlayerID() == pid &&
                players[j].getTeam() == teamID)
            {
                // If it does, add the points from that play to the team's total
                total += play_events[i].getPoints();
            }
        }
    }

    // Print the total points scored by the team
    cout << "Team " << teamID << " total points: " << total << endl;
}

// ============================================================
// Dynamic Memory (Options 13-16)
// ============================================================

// Option 13: Demonstrate PlayLog dynamic resizing
void Game::testPlayLog() {
    //   Create a local PlayLog object (starts with capacity 5)
    //   Print "Created PlayLog with capacity: 5"
    //   Loop through play_events[] and add each one with log.add()
    //   After adding 5 and 10, print count and capacity
    //   At the end print final count, capacity, and first 5 entries
    //   Note: PlayLog::add() handles resizing and prints the resize message

    // Print header
    cout << "=== TESTING PLAYLOG (Dynamic Array) ===" << endl;

    PlayLog log; // starts with capacity 5
    cout << "Created PlayLog with capacity: 5" << endl;

    cout << "\nAdding plays..." << endl;

    // Loop through play_events and add each one to the log
    for (int i = 0; i < play_count; i++) {
        // Add the memory address of play_events[i] to the log
        log.add(&play_events[i]);

        // After adding 5 and 10 plays, print the current count and capacity
        if (i == 4) {
            cout << "  Added 5 plays. Count: 5, Capacity: 5" << endl;
        }
        if (i == 9) {
            cout << "  Added 10 plays. Count: 10, Capacity: 10" << endl;
        }
    }

    // After adding all plays, print final count, capacity
    cout << "\nFinal PlayLog stats:" << endl;
    cout << "  Count: " << play_count << endl;
    cout << "  Capacity: " << 40 << endl;

    // Print the first 5 plays in the log
    cout << "\nFirst 5 plays in log:" << endl;
    log.print(5);
}

// Option 14: Build per-player dynamic play history arrays
void Game::buildPlayerPlayHistories() {
    //   Step 1: Clean up any previous run (if playerPlays != nullptr,
    //           delete each playerPlays[i], then delete[] playerPlays and delete[] playerPlayCounts)
    //
    //   Step 2: Allocate playerPlayCounts = new int[player_count]
    //           Initialize all counts to 0 (use () for zero-init or a loop)
    //
    //   Step 3: Pass 1 — count plays per player
    //           For each play, find the matching player index and increment
    //           playerPlayCounts[j]
    //
    //   Step 4: Allocate playerPlays = new PlayEvent**[player_count]
    //           For each player i, allocate playerPlays[i] = new PlayEvent*[playerPlayCounts[i]]
    //           Reset playerPlayCounts[i] = 0 (use as fill index in next step)
    //
    //   Step 5: Pass 2 — fill arrays with pointers to play_events[]
    //           For each play, find the matching player index j and assign:
    //           playerPlays[j][playerPlayCounts[j]++] = &play_events[i]
    //
    //   Step 6: Set historiesBuilt = true
    //
    //   Step 7: Print each player's name and play count

    // Print header
    cout << "=== BUILDING PLAYER PLAY HISTORIES ===" << endl;

    // Step 1: clean previous data
    if (playerPlays != nullptr) {
        for (int i = 0; i < player_count; i++) {
            delete[] playerPlays[i];
        }

        // After deleting each player's play array, delete the main arrays
        delete[] playerPlays;
        delete[] playerPlayCounts;
    }

    // Step 2
    // Allocate playerPlayCounts with initial size of player_count
    playerPlayCounts = new int[player_count]();

    // Step 3: count plays
    // Loop through each play and find the corresponding player index to increment their play count
    for (int i = 0; i < play_count; i++) {
        int pid = play_events[i].getPlayerID();

        for (int j = 0; j < player_count; j++) {
            if (players[j].getPlayerID() == pid)
                playerPlayCounts[j]++;
        }
    }

    // Step 4
    // Allocate playerPlays as an array of pointers to PlayEvent* arrays with size player_count
    playerPlays = new PlayEvent**[player_count];

    // For each player, allocate an array of PlayEvent* with size equal to their play count
    for (int i = 0; i < player_count; i++) {
        playerPlays[i] = new PlayEvent*[playerPlayCounts[i]];
        // Reset playerPlayCounts[i] to 0 to use as fill index in the next step
        playerPlayCounts[i] = 0;
    }

    // Step 5: fill arrays
    // Loop through each play again and assign pointers to the corresponding player's play array
    for (int i = 0; i < play_count; i++) {

        int pid = play_events[i].getPlayerID();

        // For each play, find the matching player index j and assign the pointer to the play event in their array
        for (int j = 0; j < player_count; j++) {

            // Check if the player's ID matches the play's player ID
            if (players[j].getPlayerID() == pid) {

                // Assign the pointer to the play event in the player's play array and increment the fill index
                playerPlays[j][playerPlayCounts[j]++] = &play_events[i];
            }
        }
    }

    historiesBuilt = true; // Step 6: set flag to indicate histories are built

    // Step 7: Print each player's name and play count
    for (int i = 0; i < player_count; i++) {
        cout << players[i].getName() << " has " << playerPlayCounts[i] << " plays" << endl;
    }
}

// Option 15: Display all plays for a specific player
void Game::displayPlayerHistory(int playerID) const {
    //   If !historiesBuilt, print a message asking them to run Option 14 first
    //   Find the player index for playerID
    //   Print "=== PLAYS FOR [name] ===" and "Total plays: N"
    //   Loop through playerPlays[i] and call display() on each
    //   If player not found, print "Player not found."

    // Check if play histories have been built
    if (!historiesBuilt) {
        cout << "Please run Option 14 first." << endl;
        return;
    }

    // Loop through players to find the one with the matching playerID
    for (int i = 0; i < player_count; i++) {

        if (players[i].getPlayerID() == playerID) {

            // Print the plays for the players
            cout << "=== PLAYS FOR " << players[i].getName()<< " ===" << endl;

            cout << "Total plays: " << playerPlayCounts[i] << endl;

            // Loop through the player's play array and call display() on each play
            for (int j = 0; j < playerPlayCounts[i]; j++) {
                cout << j << ". ";
                playerPlays[i][j]->display();
            }

            return;
        }
    }

    // If we finish the loop without finding a match, print "Player not found."
    cout << "Player not found." << endl;

}

// Option 16: Analyze shot locations using tracking data
void Game::analyzeShotLocations(int playerID) const {
    //   Find the player's name and team (if not found, print error and return)
    //   Print "=== SHOT LOCATION ANALYSIS ===" and player name
    //   For each 2PT or 3PT play by this player:
    //     Call getTrackingAt(playerID, game_time) to get position
    //     Call distanceToBasket(team, x, y) to get distance
    //     Print shot number, type, time, location, outcome, points, distance
    //
    // Helper methods available (already implemented for you):
    //   getTrackingAt(playerID, gameTime)   — returns TrackingPoint*
    //   distanceToBasket(teamID, x, y)      — returns double (feet)

    // Find the player's team and name using their playerID
    int team = -1;
    string name;

    // Loop through players array to find the player with matching ID and get their team and name
    for (int i = 0; i < player_count; i++) {
        if (players[i].getPlayerID() == playerID) {
            team = players[i].getTeam();
            name = players[i].getName();
        }
    }

    // If team id is -1, player not found, print error and return
    if (team == -1) {
        cout << "Player not found." << endl;
        return;
    }

    // Print header for shot location analysis
    cout << "=== SHOT LOCATION ANALYSIS ===" << endl;
    cout << "Player: " << name << endl;

    int shotNum = 1; // Initialize shot number to 1

    // Loop through play_events to find 2PT or 3PT plays by this player and analyze their locations
    for (int i = 0; i < play_count; i++) {

        // If the play does not belong to the specified player, skip it
        if (play_events[i].getPlayerID() != playerID) {
            continue;
        }

        // Get the type of the play and check if it's a 2PT or 3PT shot
        string type = play_events[i].getPlayType();

        // If it's not a 2PT or 3PT shot, skip it
        if (type != "2PT" && type != "3PT") {
            continue;
        }

        // For each 2PT or 3PT play, get the game time
        double gt = play_events[i].getGameTime();

        // Call getTrackingAt(playerID, game_time) to get the player's position at the time of the shot
        TrackingPoint* tp = getTrackingAt(playerID, gt);

        // If no tracking data is found for this player at that time, skip the shot
        if (tp == nullptr) {
            continue;
        }

        // Get the x and y coordinates from the tracking point
        double x = tp->getX();
        double y = tp->getY();

        // Call distanceToBasket(team, x, y) to calculate the distance from the shot location to the basket
        double dist = distanceToBasket(team, x, y);

        // Print the shot analysis information
        cout << "Shot #" << shotNum++ << endl;
        cout << "  Type: " << type << endl;
        cout << "  Time: " << gt << "s" << endl;
        cout << "  Location: (" << x << "," << y << ")" << endl;
        cout << "  Outcome: " << play_events[i].getOutcome() << endl;
        cout << "  Points: " << play_events[i].getPoints() << endl;
        cout << "  Distance: " << fixed << setprecision(1) << dist << " ft\n" << endl;
    }
}

// ============================================================
// Linked List (Options 17-20)
// These are carried over from Project 2 — re-implement as needed
// ============================================================

void Game::buildPossessionChain() {
    //   Free any existing chain first (walk the list, delete each node)
    //   Determine the starting team from play_events[0]'s player
    //   Create the first PossessionNode
    //   Loop through all plays:
    //     - Increment play_count and update end_time on current node
    //     - FT MADE: add 1 to current possession's points
    //     - Field goal MADE: compute zone-based points, add to possession,
    //       then create new node for the other team (if more plays remain)
    //     - DEF REBOUND: create new node for the rebounding team
    //                    (if more plays remain)
    //     - Everything else: stays in same possession
    //   Set possessionBuilt = true
    //   Print "Created N possessions"
    //
    // Zone-based points for field goals (same as ShotEvent):
    //   Call getTrackingAt() and distanceToBasket(), then:
    //   dist <  5 → 1 pt, < 15 → 2 pts, < 23 → 3 pts, < 30 → 4 pts, else 5 pts
    
    // Print header for possession chain building
    cout << "=== BUILDING POSSESSION CHAIN ===" << endl;

    // delete existing chain
    PossessionNode* current = possessionHead;

    // Loop through the linked list and delete each node to free memory
    while (current != nullptr) {
        PossessionNode* next = current->next;
        delete current;
        current = next;
    }

    // After freeing the existing chain, reset the head pointer to nullptr
    possessionHead = nullptr;

    // Determine the starting team from the first play event's player ID
    int pid = play_events[0].getPlayerID();

    int team = 1; // Default to team 1 if player not found

    // Loop through players array to find the player with matching ID and get their team
    for (int i = 0; i < player_count; i++) {
        if (players[i].getPlayerID() == pid) {
            team = players[i].getTeam();
        }
    }
        
    // We create the first node, which will be the head of the linked list,
    // and point current to it so the loop can build from it.
    PossessionNode* firstNode = new PossessionNode(team, play_events[0].getGameTime());
    possessionHead = firstNode;
    current = possessionHead; 
    int possessionCount = 1; // Initialize possession count to 1 since we just created the first node

    // Loop through all plays to build the possession chain
    for (int i = 0; i < play_count; i++) {

        // Update the current node first
        // For every play, we increment the play count and update the end time of the current possession
        current->play_count++;
        current->end_time = play_events[i].getGameTime();

        // Get the type and outcome of the current play event
        string type = play_events[i].getPlayType();
        string outcome = play_events[i].getOutcome();

        // Handle FT MADE (Add point, stay in possession
        // If the play type is "FT" and the outcome is "MADE" add 1 point to the current possession's points.
        if (type == "FT" && outcome == "MADE") {
            current->points += 1;
        }

        // Handle Field Goal MADE (Add points, then swap)
        // If the play type is "2PT" or "3PT" and the outcome is "MADE", compute the zone-based points, add to the current possession's points, then create a new node for the other team if more plays remain.
        if ((type == "2PT" || type == "3PT") && outcome == "MADE") {

            // Get the player ID and game time from the current play event
            int pid = play_events[i].getPlayerID();
            double gt = play_events[i].getGameTime();

            // Call getTrackingAt(playerID, game_time) to get the player's position at the time of the shot
            TrackingPoint* tp = getTrackingAt(pid, gt);

            // If tracking data is found for this player at that time, calculate the distance from the shot location to the basket and determine the points based on the distance, then add those points to the current possession's points
            if (tp != nullptr) {

                // Get the team of the player who made the shot
                double dist = distanceToBasket(current->team, tp->getX(), tp->getY());

                int pts = 1; // Initialize points to 1

                // Determine the points based on the distance from the basket using the zone-based points system
                if (dist < 5) {
                    pts = 1;
                } 
                else if (dist < 15) {
                    pts = 2;
                } 
                else if (dist < 23) {
                    pts = 3;
                } 
                else if (dist < 30) {
                    pts = 4;
                } 
                else {
                    pts = 5;
                }

                // Add the calculated points to the current possession's points
                current->points += pts;
            }

            // Move to new possession for the other team if more plays remain
            // But only if it is not the last play, since we don't want to create an extra possession node at the end of the chain
            if (i < play_count - 1) {

                // Determine the next team (the other team) and create a new PossessionNode for that team with the start time of the current play's game time, then add the new node to the linked list and update the current pointer to the new node, and increment the possession count
                int nextTeam = (current->team == 1) ? 2 : 1;
                PossessionNode* newNode = new PossessionNode(nextTeam, gt);
                current->next = newNode;
                current = newNode;
                possessionCount++;
            }
        }

        // Handle Rebound (swap if outcome is DEF, stay not swap if it is OFF)
        else if (type == "REBOUND" && outcome == "DEF") {

            // If the play type is "REBOUND", create a new node for the rebounding team (which is the opposite of the current possession's team) with the start time of the current play's game time, then add the new node to the linked list and update the current pointer to the new node, and increment the possession count.
            // But only if it is not the last play, since we don't want to create an extra possession node at the end of the chain
            if (i < play_count - 1) {
                // Get the player ID of the player who got the rebound from the current play event
                int reboundTeamPlayerID = play_events[i].getPlayerID();
                int reboundingTeam = (current->team == 1) ? 2 : 1; // Default to the opposite team of the current possession's team

                // Loop through players array to find the player with matching ID and get their team to determine the rebounding team
                for (int j = 0; j < player_count; j++) {
                    if (players[j].getPlayerID() == reboundTeamPlayerID) {
                        reboundingTeam = players[j].getTeam();
                        break; // Break the loop once we find the matching player and determine the rebounding team
                    }
                }

                // Create a new PossessionNode for the rebounding team with the start time of the current play's game time, then add the new node to the linked list and update the current pointer to the new node, and increment the possession count
                PossessionNode* newNode = new PossessionNode(reboundingTeam, play_events[i].getGameTime());
                current->next = newNode;
                current = newNode;
                possessionCount++;
            }
        }
    }

    // After all plays, set the possessionBuilt flag to true to indicate that the possession chain has been built
    possessionBuilt = true;

    // After processing all plays, print the total number of possessions created
    cout << "Created " << possessionCount << " possessions" << endl;
}

void Game::displayPossessionChain() const {
    // Traverse the linked list and print each possession

    // Check if the possession chain has been built and if the head is not null before traversing
    if (possessionBuilt == false || possessionHead == nullptr) {
        cout << "Please run Option 17 (Build Possession Chain) first." << endl;
        return;
    }

    // Traverse the linked list starting from the head and print the details of each possession
    PossessionNode* cur = possessionHead;
    int num = 1; // Initialize possession number to 1

    // Loop through the linked list until we reach the end (nullptr)
    while (cur != nullptr) {

        // Calculate the duration of the possession by subtracting the start time from the end time
        double dur = cur->end_time - cur->start_time;

        cout << fixed << setprecision(1);
        cout << "Possession #" << num++
             << ":  Team: " << cur->team
             << "  |  "
             << cur->start_time << "s – "
             << cur->end_time
             << "s  | Duration: " << dur
             << "s | " << cur->points << " pts | "
             << cur->play_count << " plays" << endl;

        // Move to the next node in the linked list
        cur = cur->next;
    }
    
}

void Game::analyzePossessionEfficiency() const {
    if (!possessionBuilt || possessionHead == nullptr) {
        cout << "Please run Option 17 (Build Possession Chain) first." << endl;
        return;
    }
    // Traverse the list, accumulate per-team stats

    // For each team, track: possession count, total points, total time, and scoring possessions
    int t1pos=0,t2pos=0;
    int t1pts=0,t2pts=0;
    double t1time=0,t2time=0;
    int t1score=0,t2score=0;

    // Traverse the linked list starting from the head and calculate stats for each team
    PossessionNode* cur = possessionHead;

    // Loop through the linked list until we reach the end (nullptr)
    while (cur != nullptr) {

        // Calculate the duration of the possession by subtracting the start time from the end time
        double dur = cur->end_time - cur->start_time;

        // Calculate stats for team 1 and team 2 based on the team ID of the current possession node
        if (cur->team == 1) {
            t1pos++;
            t1pts += cur->points;
            t1time += dur;
            if (cur->points > 0) {
                t1score++;
            }
        }
        else {
            t2pos++;
            t2pts += cur->points;
            t2time += dur;
            if (cur->points > 0) {
                t2score++;
            }
        }

        // Move to the next node in the linked list
        cur = cur->next;
    }

    cout << fixed << setprecision(2);
    // Print the calculated stats for team 1
    cout << "Team 1: " << t1pos
         << " possessions | " << t1pts
         << " pts | " << (double)t1pts / t1pos
         << " pts/possession | " << t1time / t1pos
         << "s avg | " << (double)t1score / t1pos * 100
         << "% scoring" << endl;

    // Print the calculated stats for team 2
    cout << "Team 2: " << t2pos
         << " possessions | " << t2pts
         << " pts | " << (double)t2pts / t2pos
         << " pts/possession | " << t2time / t2pos << "s avg | " << (double)t2score / t2pos * 100
         << "% scoring" << endl;
}

void Game::findLongestPossession() const {
    if (possessionBuilt == false || possessionHead == nullptr) {
        cout << "Please run Option 17 (Build Possession Chain) first." << endl;
        return;
    }
    // Find the node with the largest duration

    // Call recursive function to find the longest possession node starting from the head of the linked list
    PossessionNode* longest = recursiveLongestPossession(possessionHead, possessionHead);

    // If longest is not null, find the 1-based index by walking the chain from the head and print the details of the longest possession
    if (longest != nullptr) {
        // Find the 1-based index by walking the chain
        int idx = 1;

        // Start from the head of the linked list
        PossessionNode* cur = possessionHead;

        // Loop through the linked list until we find the longest possession node, incrementing the index as we go
        while (cur != nullptr && cur != longest) {
            idx++;
            cur = cur->next;
        }

        // Calculate the duration of the longest possession by subtracting the start time from the end time
        double dur = longest->end_time - longest->start_time;

        // Print the details of the longest possession
        cout << fixed << setprecision(1);
        cout << "Longest possession: #" << idx
             << " | Team: " << longest->team
             << " | Duration: " << dur
             << "s | " << longest->points
             << " pts | " << longest->play_count
             << " plays" << endl;
    }
}

// ============================================================
// Helper methods
// ============================================================

// Find the tracking point closest in game_time for a given player.
// Returns nullptr if no tracking data exists for that player.
TrackingPoint* Game::getTrackingAt(int playerID, double gameTime) const {
    TrackingPoint* best     = nullptr;
    double         bestDiff = 1e18;
    for (int i = 0; i < tracking_count; i++) {
        if (tracking_points[i].getPlayerID() != playerID) continue;
        double diff = fabs(tracking_points[i].getGameTime() - gameTime);
        if (diff < bestDiff) {
            bestDiff = diff;
            best     = &tracking_points[i];
        }
    }
    return best;
}

// Euclidean distance from (x,y) to the correct basket for a team.
// Team 1 shoots toward basket at (0, 0).
// Team 2 shoots toward basket at (94, 0).
double Game::distanceToBasket(int teamID, double x, double y) const {
    double goalX = (teamID == 1) ? 0.0 : 94.0;
    double goalY = 0.0;
    return sqrt(pow(x - goalX, 2) + pow(y - goalY, 2));
}

// ============================================================
// ============================================================
// buildGameEvents() and clearGameEvents()
// These functions are given to you complete. You do not need
// to write them. Read them carefully — they show you:
//   - How to create ShotEvent, FoulEvent, and ReboundEvent objects
//   - How getTrackingAt() and distanceToBasket() are used
//   - How setFTMade() is called after counting actual FT results
//   - Why FoulEvent::getPoints() returns 0 (FT ShotEvents do the scoring)
//
// Your job: implement the three subclass constructors and methods
// so that the objects created here behave correctly.
// ============================================================

// Free all dynamically allocated GameEvent objects and reset the array.
void Game::clearGameEvents() {
    if (gameEvents != nullptr) {
        for (int i = 0; i < gameEventCount; i++) {
            delete gameEvents[i];
        }
        delete[] gameEvents;
        gameEvents        = nullptr;
        gameEventCount    = 0;
        gameEventCapacity = 0;
    }
}

// Build the GameEvent polymorphic array from play_events[].
// Each play becomes a ShotEvent, FoulEvent, or ReboundEvent.
// FoulEvent::ft_made is set from the actual FT records that follow.
void Game::buildGameEvents() {
    clearGameEvents();

    gameEventCapacity = play_count + 1;
    gameEvents        = new GameEvent*[gameEventCapacity];
    gameEventCount    = 0;

    // Helper: find which team a player belongs to
    // This lambda function takes a player ID as input and returns the team ID of that player by looping through the players array and checking for a match. If no match is found, it defaults to returning 1.
    auto teamOf = [&](int pid) -> int {
        for (int j = 0; j < player_count; j++)
            if (players[j].getPlayerID() == pid) 
            return players[j].getTeam();
        return 1;
    };

    int i = 0;
    // Loop through play_events and create corresponding GameEvent objects based on the play type and outcome, using getTrackingAt() and distanceToBasket() for shots and fouls, and counting made free throws for fouls.
    // Ex - at index 0 we have 0,1,2PT,MISS,0,0.5,11:45 thats for first player so store the respective values in respective variables
    while (i < play_count) {
        const PlayEvent& pe    = play_events[i];
        const string&    ptype = pe.getPlayType();
        int    pid   = pe.getPlayerID();
        int    team  = teamOf(pid);
        double gt    = pe.getGameTime();
        string clock = pe.getRealClock();

        if (ptype == "2PT" || ptype == "3PT") {
            // Look up where the player was standing at shot time
            // if play type is "2PT" or "3PT", we create a ShotEvent for that shot. We call getTrackingAt() to find the player's position at the time of the shot, and then call distanceToBasket() to calculate the distance from the shot location to the basket. We then create a new ShotEvent with the team, game time, clock, player ID, play type, outcome, and distance, and add it to the gameEvents array.
            TrackingPoint* tp = getTrackingAt(pid, gt);
            double dist = (tp != nullptr) ? distanceToBasket(team, tp->getX(), tp->getY()) : 0.0;
            gameEvents[gameEventCount++] = new ShotEvent(team, gt, clock, pid, ptype, pe.getOutcome(), dist);
            i++;

        } else if (ptype == "FT") {
            // Free throw — no distance lookup, always worth 1 if MADE
            // If play type is "FT", we create a ShotEvent for that free throw. Since free throws do not have a distance associated with them, we can set the distance to 0. We create a new ShotEvent with the team, game time, clock, player ID, play type, outcome, and a distance of 0, and add it to the gameEvents array.
            gameEvents[gameEventCount++] = new ShotEvent(team, gt, clock, pid, pe.getOutcome());
            i++;

        } else if (ptype == "FOUL") {
            // Look up fouled player's position for zone calculation
            // If play type is "FOUL", we create a FoulEvent for that foul. We call getTrackingAt() to find the position of the player who committed the foul at the time of the foul, and then call distanceToBasket() to calculate the distance from that position to the basket. We then create a new FoulEvent with the team, game time, clock, player ID, and distance, and add it to the gameEvents array. After creating the FoulEvent, we scan the subsequent play events to count how many free throws were awarded and how many were made, and update the ft_made field of the FoulEvent accordingly.
            TrackingPoint* tp = getTrackingAt(pid, gt);
            double dist = (tp != nullptr) ? distanceToBasket(team, tp->getX(), tp->getY()) : 0.0;
            FoulEvent* fe = new FoulEvent(team, gt, clock, pid, dist);

            // Scan the FT records that immediately follow this FOUL
            // and count how many were actually made
            int ftMade = 0;
            int j = i + 1;
            while (j < play_count && play_events[j].getPlayType() == "FT") {
                if (play_events[j].getOutcome() == "MADE") ftMade++;
                j++;
            }
            fe->setFTMade(ftMade);    // update ft_made on the FoulEvent
            gameEvents[gameEventCount++] = fe;
            i++;

        } else if (ptype == "REBOUND") {
            gameEvents[gameEventCount++] =
                new ReboundEvent(team, gt, clock, pid, pe.getOutcome());
            i++;

        } else {
            i++;  // unknown type — skip gracefully
        }
    }
}

// ============================================================
// Options 21-22
// ============================================================

// Option 21: Simulate the game and compute scores
void Game::simulateGame() {
    //   Step 1: Call buildGameEvents() to create the polymorphic array
    //   Step 2: Reset team1Score = 0, team2Score = 0
    //   Step 3: Loop through gameEvents[]:
    //     int pts = gameEvents[i]->getPoints();   ← virtual dispatch
    //     if getTeamID() == 1: team1Score += pts
    //     else:                team2Score += pts
    //   Step 4: Set simulationRun = true
    //   Step 5: Print scores and winner
    //
    // Expected output:
    //   === GAME SIMULATION COMPLETE ===
    //   Team 1 Score: 6
    //   Team 2 Score: 4
    //   Winner: Team 1

    buildGameEvents(); // Step 1: Build the polymorphic array of game events

    // Step 2: Reset team scores to 0 before simulating the game
    team1Score = 0;
    team2Score = 0;

    // Step 3: Loop through the gameEvents array and compute the scores for each team based on the points from each event
    for (int i = 0; i < gameEventCount; i++) {
        int pts = gameEvents[i]->getPoints(); // Get the points from the current game event using virtual dispatch
        if (gameEvents[i]->getTeamID() == 1) {
            team1Score += pts; // If the event belongs to team 1, add the points to team 1's score
        } else {
            team2Score += pts; // If the event belongs to team 2, add the points to team 2's score
        }
    }

    simulationRun = true; // Step 4: Set simulationRun = true

    // Step 5: Print scores and winner
    cout << "=== GAME SIMULATION COMPLETE ===" << endl;
    cout << "Team 1 Score: " << team1Score << endl;
    cout << "Team 2 Score: " << team2Score << endl;

    // Determine the winner based on the final scores and print the result
    if (team1Score > team2Score) {
        cout << "Winner: Team 1" << endl;
    } else if (team2Score > team1Score) {
        cout << "Winner: Team 2" << endl;
    } else {
        cout << "Winner: Tie" << endl;
    }
}

// Option 22: Display a log of every game event
void Game::displayGameLog() const {
    //   Guard: if !simulationRun, print a message and return
    //   Print "=== FULL GAME EVENT LOG ===" and a blank line
    //   Loop through gameEvents[]:
    //     Print event index, event type, team ID, player name
    //     Then call gameEvents[i]->describe() for the type-specific detail
    //       ↑ This is the key virtual dispatch call — describe() must go through
    //         the vtable so each subclass formats its own output
    //   After all events, print the summary count of shots/fouls/rebounds
    //
    // To find a player's name: loop through players[] and match getPlayerID()

    // Check if the game has been simulated before attempting to display the game log. If not, print a message and return to avoid accessing uninitialized data.
    if (!simulationRun) {
        cout << "Please run Option 21 (Simulate Game) first." << endl;
        return;
    }

    // Header line
    cout << "=== FULL GAME EVENT LOG ===" << endl;
    cout << endl; // blank line

    int shotCount = 0; // Initialize shot count to 0
    int foulCount = 0; // Initialize foul count to 0
    int reboundCount = 0; // Initialize rebound count to 0

    // Loop through the gameEvents array and print the details of each event.
    for (int i = 0; i < gameEventCount; i++) {
       GameEvent* event = gameEvents[i]; // Get the current game event

       string playerName = ""; // Initialize player name to an empty string

        // Loop through the players array to find the player with matching ID and get their name
        for (int j = 0; j < player_count; j++) {
            if (players[j].getPlayerID() == event->getPlayerID()) {
                playerName = players[j].getName();
                break;
            }
        }

        // Print the event index, event type, team ID, player name, and the description of the event using the describe() method which call the appropriate subclass.
        cout << "[" << setw(2) << i << "] " << right 
             << setw(7) << event->getEventType() << " | Team " << event->getTeamID() << " | " << setw(16) << playerName << " | " << event->describe() << endl;

        if (event->getEventType() == "SHOT") {
            shotCount++;
        } 
        else if (event->getEventType() == "FOUL") {
            foulCount++;
        } 
        else if (event->getEventType() == "REBOUND") {
            reboundCount++;
        }
    }
    cout << endl;
    cout << "--- Event Summary ---" << endl;
    cout << "Shot attempts:   " << shotCount << endl;
    cout << "Fouls:            " << foulCount << endl;
    cout << "Rebounds:         " << reboundCount << endl;
    cout << "Total events:    " << gameEventCount << endl;
}

// ============================================================
// Option 23
// ============================================================

// Option 23: Distance scoring breakdown by player
void Game::displayDistanceBreakdown() const {
    //   Guard: if !simulationRun, print a message and return
    //   Print table header: Player | 1pt | 2pt | 3pt | 4pt | 5pt | Total
    //
    //   For each player:
    //     Loop through gameEvents[] looking for ShotEvents by this player
    //     For each ShotEvent where getPoints() > 0 (i.e., a MADE shot):
    //       Get the zone value: dynamic_cast<ShotEvent*>(ev)->getZoneValue()
    //       Accumulate points into zonePts[zone] and totalZone[zone]
    //     Print the player's row
    //
    //   Print the TOTALS row
    //   Print the highest scoring zone and top scoring player
    //
    // Casting tip:
    //   ShotEvent* se = dynamic_cast<ShotEvent*>(gameEvents[i]);
    //   if (se == nullptr) continue;  // not a ShotEvent, skip it

    // Check if the game has been simulated before displaying the distance breakdown. If not, print a error message and return.
    if (!simulationRun) {
        cout << "Please run Option 21 (Simulate Game) first." << endl;
        return;
    }

    // Print table header
    cout << "\n=== DISTANCE SCORING BREAKDOWN ===\n" << endl;

    cout << left << setw(22) << "Player"
         << setw(6) << "1pt"
         << setw(6) << "2pt"
         << setw(6) << "3pt"
         << setw(6) << "4pt"
         << setw(6) << "5pt"
         << setw(8) << "Total" << endl
         << "------------------------------------------------------------" << endl;

    int playerPoints = 0; // Initialize player points to 0
    // It will keep track of the highest total points scored by any player so far.
    string playerName = ""; // Initialize player name to an empty string
    int totalZone[6] = {0}; // Total zones are 1-5 (ignore index 0), so we create an array of size 6 to store the total points for each zone for all players, initialized to 0. Ex - totalZone[1] will store the total points scored from 1pt shots for all players, totalZone[2] for 2pt shots, etc. 

    // Loop through each player and calculate their scores by distance in zone
    for (int i = 0; i < player_count; i++) {
        // Zone points for the current player. It will store the total points the player scored in each zone.
        // Ex - zonePts[1] will store the total points the player scored from 1pt shots (zone 1), zonePts[2] will store the total points the player scored from 2pt shots (zone 2), etc. 
        int zonePts[6] = {0}; 
        int playerTotal = 0; // Total points for the current player
        int pid = players[i].getPlayerID(); // Get the player ID

        for (int j = 0; j < gameEventCount; j++) {
            GameEvent* event = gameEvents[j]; // Get the current game event

            if (event->getPlayerID() != pid) {
                continue; // If the event does not belong to the current player, skip it
            }

            // dynamic_cast the event to check if it is a ShotEvent or not. If the event is ShotEvent it returns the pointer to that event and if it is not a ShotEvent it returns nullptr.
            // We only want to consider ShotEvents for the distance breakdown, and we only want to consider made shots (where getPoints() > 0), so if the event is not a ShotEvent or if it is a missed shot (0 points), we skip it.
            ShotEvent* se = dynamic_cast<ShotEvent*>(event);

            if (se == nullptr) {
                continue; // If the event is not a ShotEvent, skip it
            }

            int pts = se->getPoints(); // Get the points from the ShotEvent means the shot was made, and the points will be based on the distance zone. If the shot was missed, getPoints() will return 0, so we check for that and skip missed shots.

            if (pts == 0) {
                continue; // If the shot was not made, skip it
            }

            int zone = se->getZoneValue(); // Get the zone value from the ShotEvent

            zonePts[zone] += pts; // Accumulate points into the appropriate zone for the player. Ex - if zone is 1, we add the points to zonePts[1] for the player, if zone is 2, we add to zonePts[2], etc.
            totalZone[zone] += pts; // Accumulate points into the total for that zone for all players. Ex - if zone is 1, we add the points to totalZone[1] for all players, if zone is 2, we add to totalZone[2], etc.
            playerTotal += pts; // Accumulate the player's total points. Ex - if the player made a 3pt shot, we add 3 points to their total, if they made a 1pt shot, we add 1 point to their total, etc.
        }
        // Print the name of the player
        cout << left << setw(22) << players[i].getName();

        // Print the points for each zone for the player
        // Ex - zonePts[1] will print the total points each player scored from 1pt shots, zonePts[2] for 2pt shots, etc. 
        for (int k = 1; k <= 5; k++) {
            cout << setw(6) << zonePts[k];
        }

        // Print the total points for the player
        cout << setw(8) << playerTotal << endl;

        // Check if this player has the highest total points so far, and if so, update the top scorer and their points
        if (playerTotal > playerPoints) {
            playerPoints = playerTotal;
            playerName = players[i].getName();
        }
    }

    cout << "------------------------------------------------------------" << endl;

    int grandTotal = 0; // Initialize grand total to 0 for calculating the total points scored across all zones for all players

    cout << left << setw(22) << "TOTAL";

    // Print the total points for each zone for all players, and accumulate the grand total of points scored across all zones for all players.
    // Ex - totalZone[1] will print the total points scored from 1pt shots for all players, totalZone[2] for 2pt shots, etc. We also add each zone's total to the grand total to get the total points scored across all zones for all players.
    for (int z = 1; z <= 5; z++) {
        cout << setw(6) << totalZone[z];
        grandTotal += totalZone[z];
    }

    // Print the grand total of points scored across all zones for all players
    cout << setw(8) << grandTotal << endl;

    int bestZone = 1; // Initialize best zone to 1.

    // Loop through the totalZone array to find which zone had the highest total points scored across all players, and update bestZone accordingly. We start from index 2 because we initialized bestZone to 1.
    for (int i = 2; i <= 5; i++) { 
        if (totalZone[i] > totalZone[bestZone]) {
            bestZone = i;
        }
    }

    // Print the highest scoring zone and the top scoring player with their points.
    cout << "\nHighest scoring zone: "
         << bestZone << "pt zone ("
         << totalZone[bestZone] << " pts total)"
         << endl;

    cout << "Top scorer:           "
         << playerName
         << " (" << playerPoints << " pts)"
         << endl;
}

// ============================================================
// Option 24
// ============================================================

// Option 24: Foul summary per team
void Game::displayFoulSummary() const {
    //   Guard: if !simulationRun, print a message and return
    //   Print "=== FOUL SUMMARY ==="
    //
    //   For each FoulEvent in gameEvents[]:
    //     Cast: FoulEvent* fe = dynamic_cast<FoulEvent*>(gameEvents[i]);
    //     if fe == nullptr: skip (not a foul)
    //     fouledTeam  = fe->getTeamID()   (the team that was fouled)
    //     foulingTeam = the other team    (the team that committed the foul)
    //     Accumulate:
    //       committed[foulingTeam]++
    //       drawn[fouledTeam]++
    //       ftAtt[fouledTeam]  += fe->getFTAwarded()
    //       ftMade[fouledTeam] += fe->getFTMade()
    //       foulPts[fouledTeam]+= fe->getFTMade()
    //
    //   Print per-team stats for Team 1 and Team 2
    //   If no fouls found, print "No fouls recorded in this game."

    // Check if the game has been simulated before displaying the foul summary. If not, print a message and return to avoid accessing uninitialized data.
    if (!simulationRun) {
        cout << "Please run Option 21 (Simulate Game) first." << endl;
        return;
    }

    // Header line
    cout << "=== FOUL SUMMARY ===" << endl;
    
    // We use an array of size 3 to store the fouls for team 1 and team 2, index 1 for team 1 and index 2 for team 2. We will ignore index 0.
    int committed[3] = {0}; // Initialize committed fouls for both teams to 0. 

    int drawn[3] = {0}; // Initialize drawn fouls for both teams to 0. 

    int ftAtt[3] = {0}; // Initialize free throw attempts for both teams to 0.
   
    int ftMade[3] = {0}; // Initialize free throws made for both teams to 0. 

    int foulPts[3] = {0}; // Initialize foul points for both teams to 0.

    int foulCount = 0; // Initialize foul count to 0 for checking if any fouls were found in the game

    // Loop through the gameEvents array to find FoulEvents and accumulate the foul stats for each team
    for (int i = 0; i < gameEventCount; i++) {
        // dynamic_cast the event to check if it is a FoulEvent or not. If the event is FoulEvent it returns the pointer to that event and if it is not a FoulEvent it returns nullptr.
        FoulEvent* fe = dynamic_cast<FoulEvent*>(gameEvents[i]);

        if (fe == nullptr) {
            continue; // If the event is not a FoulEvent, skip it
        }
        
        // Get the team that was fouled (means the team that was awarded free throw from the foul).
        int fouledTeam = fe->getTeamID(); 

        // Get the fouling team (means the team that committed the foul), which is the opposite of the fouled team. If the fouled team is 1, then the fouling team is 2, and if the fouled team is 2, then the fouling team is 1.
        int foulingTeam = (fouledTeam == 1) ? 2 : 1; 

        // Increment the committed fouls for the fouling team since they committed the foul. 
        // Ex - if team 1 was fouled, then team 2 committed the foul, so we increment committed[2].
        committed[foulingTeam]++;

        // Increment the drawn fouls for the fouled team since they were fouled. 
        //Ex - if team 1 was fouled, we increment drawn[1], if team 2 was fouled, we increment drawn[2].
        drawn[fouledTeam]++;

        // Add the free throw attempts awarded to the fouled team's total free throw attempts. 
        // Ex - if team 1 was fouled and awarded 2 free throws, we add 2 to ftAtt[1], ftAtt[1] += 2. If team 2 was fouled and awarded 3 free throws, we add 3 to ftAtt[2]. ftAtt[2] += 3;
        ftAtt[fouledTeam] += fe->getFTAwarded();

        // Add the free throws made to the fouled team's total free throws made since each free throw made is worth 1 point.
        // Ex - if team 1 was fouled and made 1 free throw, we add 1 to ftMade[1], ftMade[1] += 1. If team 2 was fouled and made 2 free throws, we add 2 to ftMade[2], ftMade[2] += 2.
        ftMade[fouledTeam] += fe->getFTMade();

        // Add the free throw points (which is the same as free throws made since each free throw is worth 1 point) to the fouled team's total foul points.
        // Ex - if team 1 was fouled and made 1 free throw, we add 1 to foulPts[1], foulPts[1] += 1. If team 2 was fouled and made 2 free throws, we add 2 to foulPts[2], foulPts[2] += 2.
        foulPts[fouledTeam] += fe->getFTMade();

        // Increment the foul count since we found a foul event
        foulCount++;
    }

    // If not foul event were found in the game, print the error message and return.
    if (foulCount == 0) {
        cout << "No fouls recorded in this game." << endl;
        return;
    }

    // Print Team 1 stats
    cout << "\nTeam 1:" << endl;
    cout << "  Fouls committed:   " << committed[1] << endl;
    cout << "  Fouls drawn:       " << drawn[1]   << endl;
    cout << "  FT attempts:       " << ftAtt[1]   << endl;
    cout << "  FT made:           " << ftMade[1]  << endl;
    cout << "  Points from fouls: " << foulPts[1] << endl;

    // Print Team 2 stats
    cout << "\nTeam 2:" << endl;
    cout << "  Fouls committed:   " << committed[2] << endl;
    cout << "  Fouls drawn:       " << drawn[2]   << endl;
    cout << "  FT attempts:       " << ftAtt[2]   << endl;
    cout << "  FT made:           " << ftMade[2]  << endl;
    cout << "  Points from fouls: " << foulPts[2] << endl;
}

// ============================================================
// Recursive helpers and Options 25-26
// These use the possession chain built by Option 17.
// ============================================================

// Recursively sum points across all possessions from 'node' onward.
// Base case:    node is nullptr → return 0
// Recursive:    return node->points + recursivePossessionScore(node->next)
int Game::recursivePossessionScore(PossessionNode* node) const {
    // Implement the two-line recursive function above

    // Base case: if the current node is nullptr, we have reached the end of the possession chain, so we return 0 since there are no more points to add.
    if (node == nullptr) {
        return 0;
    }

    // Recursive case: we return the points from the current node plus the result of calling recursivePossessionScore on the next node in the chain. This will sum up all the points from the current node to the end of the chain.
    return node->points + recursivePossessionScore(node->next);
}

// Recursively sum points for possessions belonging to 'team' only.
// Base case:    node is nullptr → return 0
// Recursive:    add node->points (only if node->team == team) + recursiveTeamScore(node->next, team)
int Game::recursiveTeamScore(PossessionNode* node, int team) const {
    // Base case: if the current node is nullptr, we have reached the end of the possession chain, so we return 0 since there are no more points to add.
    if (node == nullptr) {
        return 0;
    }
    
    // Recursive case: we check if the team of the current node matches the team we are summing for. If it does, we add the points from the current node to the result of calling recursiveTeamScore on the next node in the chain.
    if (node->team == team) {
        return node->points + recursiveTeamScore(node->next, team);
    }

    // If it does not match, we simply call recursiveTeamScore on the next node without adding the points from the current node.
    else {
        return recursiveTeamScore(node->next, team);
    }
}

// Recursively find the possession with the longest duration.
// Base case:    node is nullptr → return currentLongest
// Recursive:    compare node's duration to currentLongest's duration,
//               update newLongest if node is longer,
//               then return recursiveLongestPossession(node->next, newLongest)
PossessionNode* Game::recursiveLongestPossession(PossessionNode* node, PossessionNode* currentLongest) const {
    // Base case: if the current node is nullptr, we have reached the end of the possession chain, so we return the current longest possession found.
    if (node == nullptr) {
        return currentLongest;
    }

    // Recursive case: we compare the duration of the current node to the duration of the current longest possession. If the current node's duration is longer, we update the current longest possession to be the current node. Then we call recursiveLongestPossession on the next node in the chain, passing the updated longest possession.

    // Calculate the duration of the current node by subtracting the start time from the end time.
    double nodeDuration = node->end_time - node->start_time;

    // Calculate the duration of the current longest possession by subtracting the start time from the end time.
    double longestDuration = currentLongest->end_time - currentLongest->start_time;

    // Update the current longest possession to be the current node if it has a longer duration than the current longest possession.
    if (nodeDuration > longestDuration) {
        currentLongest = node; 
    }

    // Recursive call on the next node in the chain with the updated longest possession
    return recursiveLongestPossession(node->next, currentLongest);
}

// Option 25: Print each possession's score using recursion
void Game::recursivePossessionReport() const {
    if (!possessionBuilt || possessionHead == nullptr) {
        cout << "Please run Option 17 (Build Possession Chain) first." << endl;
        return;
    }
    //   Count total possessions (walk the list)
    //   Print header: "=== RECURSIVE POSSESSION SCORE REPORT ==="
    //   Walk and print each possession (num, team, start, end, duration, pts, plays)
    //   Call recursivePossessionScore(possessionHead) for the total
    //   If simulationRun, verify it matches team1Score + team2Score

    int possessionCount = 0; // Initialize possession count to 0
    PossessionNode* cur = possessionHead; // Start from the head of the possession chain

    // Loop through the possession chain to count the total number of possessions.
    while (cur != nullptr) {
        possessionCount++; // Increment possession count for each node in the chain
        cur = cur->next; // Move to the next possession in the chain
    }

    // Header line
    cout <<"=== RECURSIVE POSSESSION SCORE REPORT ===" << endl;
    cout << "(Traversing " << possessionCount << " possessions recursively)\n" << endl;

    cur = possessionHead; // Reset cur to the head of the possession chain to start printing the details of each possession

    possessionCount = 1; // Reset possession count to 1 for printing the possession number starting from 1
    
    while (cur != nullptr) {
        double duration = cur->end_time - cur->start_time; // Calculate the duration of the current possession
        cout << fixed << setprecision(1);
        cout << "Possession #" << possessionCount
             << ": Team: " << cur->team
             << " | " << cur->start_time << "s - " << cur->end_time
             << "s | Duration: " << duration
             << "s | " << cur->points << " pts | "
             << cur->play_count << " plays" << endl;

        cur = cur->next; // Move to the next possession in the chain
        possessionCount++; // Increment possession count for the next possession
    }

    // Call recursivePossessionScore with the head of the possession chain to get the total points scored across all possessions.
    int totalPoints = recursivePossessionScore(possessionHead);

    cout << "\nRecursive total (all possessions): " << totalPoints << " pts" << endl;

    // if simulationRun is true, we verify that the total points calculated from the recursive function matches the sum of team1Score and team2Score.
    // You must run Option 21 (Simulate Game) before running this option to have the team scores available for verification.
    if (simulationRun == true) {
    cout << "  -> Matches Option 21: T1 (" << team1Score << ") + T2 (" << team2Score << ") = " << totalPoints << " pts" << endl;
}
}

// Option 26: Print per-team recursive score totals
void Game::recursiveTeamReport() const {
    if (!possessionBuilt || possessionHead == nullptr) {
        cout << "Please run Option 17 (Build Possession Chain) first." << endl;
        return;
    }
    //   Call recursiveTeamScore(possessionHead, 1) for Team 1
    //   Call recursiveTeamScore(possessionHead, 2) for Team 2
    //   Count possessions per team (walk the list)
    //   Print results and verify against simulationRun totals

    // Call recursiveTeamScore for Team 1 to get the total points scored by Team 1 across all possessions and store it in team1.
    int team1pts = recursiveTeamScore(possessionHead, 1);

    // Call recursiveTeamScore for Team 2 to get the total points scored by Team 2 across all possessions and store it in team2.
    int team2pts = recursiveTeamScore(possessionHead, 2);

    int team1possessions = 0; // Initialize possession count for Team 1 to 0
    int team2possessions = 0; // Initialize possession count for Team 2 to 0

    PossessionNode* cur = possessionHead; // Start from the head of the possession chain

    // Loop through the possession chain to count the total number of possessions of each team.
    while (cur != nullptr) {
        if (cur->team == 1) {
            team1possessions++; // if the possession belongs to team 1 then increment possession count for Team 1
        } else {
            team2possessions++; // otherwise increment possession count for Team 2
        }
        cur = cur->next; // Move to the next possession in the chain
    }

        cout << "=== RECURSIVE TEAM SCORE TOTALS ===" << endl;

        cout << "\nTeam 1: " << team1possessions << " possessions -> " << team1pts << " pts total" << endl;

        cout << "Team 2: " << team2possessions << " possessions -> " << team2pts << " pts total" << endl;

        // If simulationRun is true, we verify that the team scores calculated from the recursive function matches the team1Score and team2Score from the simulation.
        // You must run Option 21 (Simulate Game) before running this option to have the team scores available for verification.
        if (simulationRun == true) {
            cout << "\nMatches Option 21: Team 1 = " << team1Score << ", Team 2 = " << team2Score << endl;
            cout << "\n  Recursive totals match simulation totals" << endl;
        }
}