// Battlefield.h
// Standalone Battlefield class for Robot War assignment

#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

const char EMPTY_CELL = '.';

class Battlefield {
private:
    int width;
    int height;
    std::vector<std::vector<char>> grid;
    std::map<std::string, std::pair<int, int>> robotPositions;

public:
    Battlefield(int w, int h) : width(w), height(h), grid(h, std::vector<char>(w, EMPTY_CELL)) {}

    // Place a robot if the position is free and valid
    bool placeRobot(const std::string& name, int x, int y) {
        if (isInside(x, y) && !isOccupied(x, y)) {
            grid[y][x] = name[0]; // First letter of the robot's name
            robotPositions[name] = {x, y};
            return true;
        }
        return false;
    }

    // Remove a robot from its position
    void removeRobot(const std::string& name) {
        auto it = robotPositions.find(name);
        if (it != robotPositions.end()) {
            int x = it->second.first;
            int y = it->second.second;
            if (isInside(x, y)) grid[y][x] = EMPTY_CELL;
            robotPositions.erase(it);
        }
    }

    // Move robot to new position if available
    bool moveRobot(const std::string& name, int newX, int newY) {
        if (!isInside(newX, newY) || isOccupied(newX, newY)) return false;
        auto it = robotPositions.find(name);
        if (it == robotPositions.end()) return false;

        removeRobot(name);
        return placeRobot(name, newX, newY);
    }

    // Get robot position
    std::pair<int, int> getRobotPosition(const std::string& name) const {
        auto it = robotPositions.find(name);
        if (it != robotPositions.end()) return it->second;
        return {-1, -1};
    }

    // Display battlefield
    void display() const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                std::cout << grid[y][x];
            }
            std::cout << '\n';
        }
    }

    // Check if coordinates are inside the battlefield
    bool isInside(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    // Check if a cell is occupied
    bool isOccupied(int x, int y) const {
        return grid[y][x] != EMPTY_CELL;
    }
};

#endif // BATTLEFIELD_H boi
