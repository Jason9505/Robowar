// Modified GenericRobot.h
#ifndef GENERICROBOT_H
#define GENERICROBOT_H

#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <functional>

class Battlefield; // Forward declaration

class GenericRobot {
private:
    std::string name;
    int x, y;
    int shells;
    int lives;
    Battlefield* battlefield; // Reference to interact with the battlefield
    std::function<void(GenericRobot*)> onDestroyedCallback;

public:
    GenericRobot(const std::string& name, int x, int y, Battlefield* bf)
        : name(name), x(x), y(y), shells(10), lives(3), battlefield(bf) {}

    std::string getName() const { return name; }
    int getShells() const { return shells; }
    int getLives() const { return lives; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
    void decrementLives() { if (lives > 0) lives--; }

    void setOnDestroyedCallback(std::function<void(GenericRobot*)> callback) {
        onDestroyedCallback = callback;
    }

    void think() {
        std::cout << name << " is thinking..." << std::endl;
    }

    void look(int dx, int dy) {
        int lx = x + dx;
        int ly = y + dy;
        std::cout << name << " looks at (" << lx << ", " << ly << ")" << std::endl;
    }

    void fire(int dx, int dy) {
        if (shells <= 0) {
            std::cout << name << " has no shells left and self-destructs!" << std::endl;
            battlefield->removeRobot(name);
            if (onDestroyedCallback) onDestroyedCallback(this);
            return;
        }

        int fx = x + dx;
        int fy = y + dy;
        shells--;
        std::cout << name << " fires at (" << fx << ", " << fy << "). Shells left: " << shells << std::endl;
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
        std::cout << name << " chooses an upgrade!" << std::endl;
    }

    void takeTurn() {
        think();

        int look_dx = rand() % 3 - 1;
        int look_dy = rand() % 3 - 1;
        look(look_dx, look_dy);

        int fire_dx = rand() % 3 - 1;
        int fire_dy = rand() % 3 - 1;
        if (fire_dx != 0 || fire_dy != 0) {
            fire(fire_dx, fire_dy);
        }

        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

#endif // GENERICROBOT_H