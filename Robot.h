// Robot.h
#ifndef ROBOT_H
#define ROBOT_H

#include "GenericRobot.h"
#include "Battlefield.h"
#include <queue>
#include <random>

class Robot {
private:
    Battlefield battlefield;
    std::vector<GenericRobot*> robots;
    std::queue<GenericRobot*> respawnQueue;
    std::mt19937 rng{std::random_device{}()};

public:
    Robot(int width, int height) : battlefield(width, height) {}

    void addRobot(GenericRobot* robot) {
        robots.push_back(robot);
        robot->setOnDestroyedCallback([this](GenericRobot* r) { robotDestroyed(r); });
        battlefield.placeRobot(robot->getName(), robot->getPosition().first, robot->getPosition().second);
    }

    void robotDestroyed(GenericRobot* robot) {
        robot->decrementLives();
        if (robot->getLives() > 0) {
            std::cout << robot->getName() << " will reenter the battlefield later." << std::endl;
            respawnQueue.push(robot);
        } else {
            std::cout << robot->getName() << " is permanently destroyed." << std::endl;
        }
    }

    void respawnOneRobot() {
        if (respawnQueue.empty()) return;

        GenericRobot* robot = respawnQueue.front();
        respawnQueue.pop();

        for (int i = 0; i < 100; ++i) {
            int x = rng() % battlefield.getWidth();
            int y = rng() % battlefield.getHeight();

            if (!battlefield.isOccupied(x, y)) {
                battlefield.placeRobot(robot->getName(), x, y);
                robot->setPosition(x, y);
                std::cout << robot->getName() << " reenters at (" << x << ", " << y << ")\n";
                return;
            }
        }

        std::cout << "Failed to find a space for " << robot->getName() << " to respawn.\n";
    }

    void takeTurn() {
        for (GenericRobot* robot : robots) {
            robot->takeTurn();
        }
        respawnOneRobot();
        battlefield.display();
    }

    Battlefield& getBattlefield() { return battlefield; }
};

#endif // ROBOT_H