// ============================================================
// main.cpp
//
/// Application entry point.
// Loads basketball datasets and launches the analytics console.
// ============================================================
#include "Game.h"
#include <iostream>
#include <string>
using namespace std;

// Print the full menu
void printMenu() {
    cout << "\n========================================";
    cout << " BASKETBALL ANALYTICS SYSTEM ";
    cout << "========================================" << endl;

    cout << "\n--- DATA ACCESS ---" << endl;
    cout << " 1.  Display all players" << endl;
    cout << " 2.  Get player by ID" << endl;
    cout << " 3.  Get play by index" << endl;
    cout << " 4.  Get tracking point by index" << endl;
    cout << " 5.  Count total plays" << endl;
    cout << " 6.  Count total tracking points" << endl;
    cout << " 7.  Find player by name" << endl;
    cout << " 8.  Show file load summary" << endl;

    cout << "\n--- CALCULATIONS ---" << endl;
    cout << " 9.  Player shot count" << endl;
    cout << " 10. Player total points" << endl;
    cout << " 11. Player shooting percentage" << endl;
    cout << " 12. Team total points" << endl;

    cout << "\n--- MEMORY MANAGEMENT ---" << endl;
    cout << " 13. Test PlayLog (dynamic array)" << endl;
    cout << " 14. Build player play histories" << endl;
    cout << " 15. Display player history" << endl;
    cout << " 16. Analyze shot locations" << endl;

    cout << "\n--- POSSESSION ANALYSIS ---" << endl;
    cout << " 17. Build possession chain" << endl;
    cout << " 18. Display possession chain" << endl;
    cout << " 19. Analyze possession efficiency" << endl;
    cout << " 20. Find longest possession" << endl;

    cout << "\n--- GAME SIMULATION ---" << endl;
    cout << " 21. Simulate game" << endl;
    cout << " 22. Display full game log" << endl;

    cout << "\n--- DISTANCE SCORING ---" << endl;
    cout << " 23. Distance scoring breakdown by player" << endl;

    cout << "\n--- FOUL SIMULATION ---" << endl;
    cout << " 24. Foul summary" << endl;

    cout << "\n--- ADVANCED REPORTS ---" << endl;
    cout << " 25. Recursive possession score report" << endl;
    cout << " 26. Recursive team score totals" << endl;

    cout << "\n 0 or 27. Exit" << endl;
    cout << "\nEnter choice: ";
}

int main() {
    Game game;

    // Load all three data files on startup
    cout << "Loading data files..." << endl;
    game.readPlayerFile("player_info.txt");
    game.readPlayFile("play_events.txt");
    game.readTrackingFile("tracking_data.txt");
    game.printLoadSummary();

    int choice;
    do {
        printMenu();

        // Read choice; handle non-integer input gracefully
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }
        cin.ignore(1000, '\n');

        if (choice == 1) {
            game.displayAllPlayers();

        } else if (choice == 2) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.getPlayerByID(id);

        } else if (choice == 3) {
            int idx;
            cout << "Enter play index: ";
            cin >> idx;
            cin.ignore(1000, '\n');
            game.getPlayByIndex(idx);

        } else if (choice == 4) {
            int idx;
            cout << "Enter tracking point index: ";
            cin >> idx;
            cin.ignore(1000, '\n');
            game.getTrackingByIndex(idx);

        } else if (choice == 5) {
            game.countPlays();

        } else if (choice == 6) {
            game.countTracking();

        } else if (choice == 7) {
            string name;
            cout << "Enter player name: ";
            getline(cin, name);
            game.findPlayerByName(name);

        } else if (choice == 8) {
            game.displayLoadSummary();

        } else if (choice == 9) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.getPlayerShotCount(id);

        } else if (choice == 10) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.getPlayerTotalPoints(id);

        } else if (choice == 11) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.getPlayerShootingPct(id);

        } else if (choice == 12) {
            int team;
            cout << "Enter team ID (1 or 2): ";
            cin >> team;
            cin.ignore(1000, '\n');
            game.getTeamTotalPoints(team);

        } else if (choice == 13) {
            game.testPlayLog();

        } else if (choice == 14) {
            game.buildPlayerPlayHistories();

        } else if (choice == 15) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.displayPlayerHistory(id);

        } else if (choice == 16) {
            int id;
            cout << "Enter player ID: ";
            cin >> id;
            cin.ignore(1000, '\n');
            game.analyzeShotLocations(id);

        } else if (choice == 17) {
            game.buildPossessionChain();

        } else if (choice == 18) {
            game.displayPossessionChain();

        } else if (choice == 19) {
            game.analyzePossessionEfficiency();

        } else if (choice == 20) {
            game.findLongestPossession();

        } else if (choice == 21) {
            game.simulateGame();

        } else if (choice == 22) {
            game.displayGameLog();

        } else if (choice == 23) {
            game.displayDistanceBreakdown();

        } else if (choice == 24) {
            game.displayFoulSummary();

        } else if (choice == 25) {
            game.recursivePossessionReport();

        } else if (choice == 26) {
            game.recursiveTeamReport();

        } else if (choice == 27) {
            cout << "Goodbye!" << endl;

        } else if (choice == 0) {
            cout << "Goodbye!" << endl;

        } else {
            cout << "Invalid choice. Please enter 0-27." << endl;
        }

    } while (choice != 27 && choice != 0);
    return 0;
}
