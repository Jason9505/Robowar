/**********|**********|**********|
Program: Battlefield.h
Course: Data Structures and Algorithms
Trimester: 2410
Name: Jason Hean Qi Shen
ID: 242UC244FW
Lecture Section: TC3L
Tutorial Section: TT12L
Email: jason.hean.qi@studen.mmu.edu.my
Phone: 016-5556355
**********|**********|**********/
#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <memory>

class GenericRobot;

const char EMPTY_CELL = '.';  // Character representing an empty cell on the battlefield

class Battlefield {
private:
    int width;  // Width of the battlefield grid
    int height; // Height of the battlefield grid
    std::vector<std::vector<char>> grid;  // 2D grid representing the battlefield
    std::map<std::string, std::pair<int, int>> robotPositions;  // Map of robot names to their positions
    std::map<std::string, std::weak_ptr<GenericRobot>> robotReferences; 

public:
    // Constructors initialize the battlefield with empty cells
    Battlefield() : width(20), height(20), grid(20, std::vector<char>(20, EMPTY_CELL)) {}
    Battlefield(int w, int h) : width(w), height(h), grid(h, std::vector<char>(w, EMPTY_CELL)) {}

    // Places a robot at specified coordinates if the position is valid and unoccupied
    bool placeRobot(const std::string& name, int x, int y) {
        if (isInside(x, y) && !isOccupied(x, y)) {
            grid[y][x] = name[0];  // Use first character of name as grid representation
            robotPositions[name] = {x, y};
            return true;
        }
        return false;
    }

    // Removes a robot from the battlefield by name
    void removeRobot(const std::string& name) {
        auto it = robotPositions.find(name);
        if (it != robotPositions.end()) {
            int x = it->second.first;
            int y = it->second.second;
            if (isInside(x, y)) grid[y][x] = EMPTY_CELL;
            robotPositions.erase(it);
        }
    }

    void registerRobotReference(const std::string& name, std::shared_ptr<GenericRobot> robot) {
        robotReferences[name] = robot;
    }

    std::shared_ptr<GenericRobot> getRobotReference(const std::string& name) {
        auto it = robotReferences.find(name);
        return (it != robotReferences.end()) ? it->second.lock() : nullptr;
    }

    // Moves a robot to new coordinates if the position is valid and unoccupied
    bool moveRobot(const std::string& name, int newX, int newY) {
        if (!isInside(newX, newY) || isOccupied(newX, newY)) return false;
        auto it = robotPositions.find(name);
        if (it == robotPositions.end()) return false;

        removeRobot(name);
        return placeRobot(name, newX, newY);
    }

    // Returns the position of a robot by name, or (-1,-1) if not found
    std::pair<int, int> getRobotPosition(const std::string& name) const {
        auto it = robotPositions.find(name);
        if (it != robotPositions.end()) return it->second;
        return {-1, -1};
    }

    // Returns the complete map of robot positions
    const std::map<std::string, std::pair<int, int>>& getRobotPositions() const {
        return robotPositions;
    }

    // Displays the battlefield grid with coordinates
    void display() const {
        std::cout << "\n" << width << "x" << height << " Battlefield:\n";
        
        // Print column numbers (x-axis)
        std::cout << "   ";
        for (int x = 0; x < width; x++) {
            std::cout << std::setw(2) << x % 10;  // Single-digit display for simplicity
        }
        std::cout << "\n";

        // Print each row with row number (y-axis)
        for (int y = 0; y < height; y++) {
            std::cout << std::setw(2) << y << " ";
            
            // Print grid cells
            for (int x = 0; x < width; x++) {
                std::cout << grid[y][x] << " ";
            }
            std::cout << "\n";
        }
    }

    // Helper functions to check position validity and occupancy
    bool isInside(int x, int y) const { return x >= 0 && x < width && y >= 0 && y < height; }
    bool isOccupied(int x, int y) const { return grid[y][x] != EMPTY_CELL; }

    // Accessors for battlefield dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif // BATTLEFIELD_H