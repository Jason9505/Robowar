#ifndef ROBOTUPGRADES_H
#define ROBOTUPGRADES_H

#include "Robot.h"

class JumpBot : public GenericRobot {
private:
    int jumpCount;
    int battlefieldWidth;
    int battlefieldHeight;

public:
    JumpBot(const std::string& name, int x, int y, Battlefield* bf) 
        : GenericRobot(name, x, y, bf), jumpCount(3), 
          battlefieldWidth(bf->getWidth()), battlefieldHeight(bf->getHeight()) {
        upgraded = true;
    }

    void takeTurn() override {
        if (jumpCount > 0) {
            think();
            // Jump instead of normal movement
            int newX = rand() % battlefieldWidth;
            int newY = rand() % battlefieldHeight;
            
            if (getBattlefield()->moveRobot(getName(), newX, newY)) {
                setPosition(newX, newY);
                jumpCount--;
                std::cout << getName() << " jumped to (" << newX << ", " << newY 
                          << "). " << jumpCount << " jumps remaining.\n";
            } else {
                std::cout << getName() << " failed to jump.\n";
            }
            
            // Still can fire
            int fire_dx = 0, fire_dy = 0;
            do {
                fire_dx = rand() % 3 - 1;
                fire_dy = rand() % 3 - 1;
            } while (fire_dx == 0 && fire_dy == 0);
            fire(fire_dx, fire_dy);
        } else {
            GenericRobot::takeTurn();
        }
    }
};

class SemiAutoBot : public GenericRobot {
private:
    const double SEMI_AUTO_HIT_PROBABILITY = 0.6;

public:
    SemiAutoBot(const std::string& name, int x, int y, Battlefield* bf) 
        : GenericRobot(name, x, y, bf) {
        setShells(15); // Give more shells
        upgraded = true;
    }

    void fire(int dx, int dy) override {
        if (getShells() <= 0) {
            std::cout << getName() << " has no shells left and cannot fire\n";
            return;
        }

        // Fire 3-round burst
        for (int i = 0; i < 3 && getShells() > 0; i++) {
            GenericRobot::fire(dx, dy);
        }
    }
};

class TrackBot : public GenericRobot {
private:
    int trackersLeft;
    std::map<std::string, std::pair<int, int>> trackedRobots;

public:
    TrackBot(const std::string& name, int x, int y, Battlefield* bf) 
        : GenericRobot(name, x, y, bf), trackersLeft(3) {
        upgraded = true;
    }

    void takeTurn() override {
        think();

        // First try to track enemies
        if (trackersLeft > 0) {
            const auto& positions = getBattlefield()->getRobotPositions();
            for (const auto& entry : positions) {
                if (entry.first != getName() && trackedRobots.find(entry.first) == trackedRobots.end()) {
                    trackedRobots[entry.first] = entry.second;
                    trackersLeft--;
                    std::cout << getName() << " is now tracking " << entry.first 
                              << " at (" << entry.second.first << ", " << entry.second.second << ")\n";
                    break;
                }
            }
        }

        // Fire at tracked enemies if possible
        for (const auto& entry : trackedRobots) {
            auto pos = entry.second;
            int dx = pos.first - getPosition().first;
            int dy = pos.second - getPosition().second;
            
            // Fire if within range
            if (abs(dx) <= 1 && abs(dy) <= 1) {
                fire(dx, dy);
                break;
            }
        }

        // Normal movement
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

#endif // ROBOTUPGRADES_H