#ifndef GENERICROBOT_H
#define GENERICROBOT_H

#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

class Battlefield; // Forward declaration

class GenericRobot {
private:
    std::string name;
    int x, y;
    int shells;
    Battlefield* battlefield; // Reference to interact with the battlefield

public:
    GenericRobot(const std::string& name, int x, int y, Battlefield* bf)
        : name(name), x(x), y(y), shells(10), battlefield(bf) {}

    std::string getName() const { return name; }
    int getShells() const { return shells; }
    std::pair<int, int> getPosition() const { return {x, y}; }

    void think() {
        std::cout << name << " is thinking..." << std::endl;
    }

    void look(int dx, int dy) {
        int lx = x + dx;
        int ly = y + dy;
        std::cout << name << " looks at (" << lx << ", " << ly << ")" << std::endl;
        // This could return more info later.
    }

    void fire(int dx, int dy) {
        if (shells <= 0) {
            std::cout << name << " has no shells left and self-destructs!" << std::endl;
            battlefield->removeRobot(name);
            return;
        }

        int fx = x + dx;
        int fy = y + dy;
        shells--;
        std::cout << name << " fires at (" << fx << ", " << fy << "). Shells left: " << shells << std::endl;

        // In a real implementation, you would check for and damage enemy robots here.
    }

    void move(int dx, int dy) {
        int newX = x + dx;
        int newY = y + dy;

        if (battlefield->moveRobot(name, newX, newY)) {
            x = newX;
            y = newY;
            std::cout << name << " moves to (" << x << ", " << y << ")" << std::endl;
        } else {
            std::cout << name << " failed to move to (" << newX << ", " << newY << ")" << std::endl;
        }
    }

    void chooseUpgrade() {
        // Placeholder for upgrade system
        std::cout << name << " chooses an upgrade!" << std::endl;
    }

    void takeTurn() {
        think();

        // Look around randomly for now
        int look_dx = rand() % 3 - 1;
        int look_dy = rand() % 3 - 1;
        look(look_dx, look_dy);

        // Fire randomly at neighbors
        int fire_dx = rand() % 3 - 1;
        int fire_dy = rand() % 3 - 1;
        if (fire_dx != 0 || fire_dy != 0) {
            fire(fire_dx, fire_dy);
        }

        // Move randomly or stay in place
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

#endif // GENERICROBOT_H
