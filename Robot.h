// Robot.h
#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <functional>
#include <random>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <algorithm>  // Added for std::find_if
#include "Battlefield.h"

// Forward declarations for upgrade classes
class JumpBot;
class SemiAutoBot;
class TrackBot;

class GenericRobot {
protected:
    std::string name;
    int x, y;
    int shells;
    int lives;
    Battlefield* battlefield;
    std::function<void(GenericRobot*)> onDestroyedCallback;
    bool upgraded = false;

public:
    bool hasUpgrade() const { return upgraded; }
    GenericRobot(const std::string& name, int x, int y, Battlefield* bf)
        : name(name), x(x), y(y), shells(10), lives(3), battlefield(bf) {}
    virtual ~GenericRobot() = default;

    std::string getName() const { return name; }
    int getShells() const { return shells; }
    int getLives() const { return lives; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
    void setShells(int s) { shells = s; }
    void decrementLives() { if (lives > 0) lives--; }

    void setOnDestroyedCallback(std::function<void(GenericRobot*)> callback) {
        onDestroyedCallback = callback;
    }

    virtual void think() {
        std::cout << name << " is thinking..." << std::endl;
    }

    virtual void look(int dx, int dy) {
        int lx = x + dx;
        int ly = y + dy;
        std::cout << name << " looks at (" << lx << ", " << ly << ")" << std::endl;
    }

    virtual void move(int dx, int dy) {
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

    virtual std::shared_ptr<GenericRobot> applyRandomUpgrade() {
        if (upgraded) return nullptr;
        
        upgraded = true;
        int choice = rand() % 3;
        std::cout << name << " is receiving an upgrade! ";
        
        switch(choice) {
            case 0:
                std::cout << "Becoming a JumpBot!\n";
                return std::make_shared<JumpBot>(name, x, y, battlefield);
            case 1:
                std::cout << "Becoming a SemiAutoBot!\n";
                return std::make_shared<SemiAutoBot>(name, x, y, battlefield);
            case 2:
                std::cout << "Becoming a TrackBot!\n";
                return std::make_shared<TrackBot>(name, x, y, battlefield);
        }
        return nullptr;
    }

    Battlefield* getBattlefield() const { return battlefield; }

    virtual void fire(int dx, int dy) {
        if (dx == 0 && dy == 0) {
            std::cout << name << " refuses to fire at itself (0,0 offset)\n";
            return;
        }

        if (shells <= 0) {
            std::cout << name << " has no shells left and cannot fire\n";
            return;
        }

        int fx = x + dx;
        int fy = y + dy;
        shells--;
        
        std::cout << name << " fires at (" << fx << ", " << fy << "). Shells left: " << shells << "\n";

        bool killedSomeone = false;
        if ((rand() % 100) > 70) {
            const auto& positions = battlefield->getRobotPositions();
            for (const auto& entry : positions) {
                const std::string& rname = entry.first;
                const auto& pos = entry.second;
                if (pos.first == fx && pos.second == fy) {
                    std::cout << "Direct hit! " << rname << " at (" << fx << ", " << fy << ") was destroyed!\n";
                    battlefield->removeRobot(rname);
                    killedSomeone = true;
                    if (onDestroyedCallback) {
                        onDestroyedCallback(this);
                    }
                    break;
                }
            }
        } else {
            std::cout << "The shot missed!\n";
        }

        // Grant upgrade if this robot killed someone and doesn't have an upgrade yet
        if (killedSomeone && !hasUpgrade()) {
            auto upgradedRobot = applyRandomUpgrade();
            if (upgradedRobot) {
                // Need to replace this robot in the manager
                if (onDestroyedCallback) {
                    onDestroyedCallback(this);
                }
            }
        }
    }

    virtual void takeTurn() {
        think();

        int look_dx = rand() % 3 - 1;
        int look_dy = rand() % 3 - 1;
        look(look_dx, look_dy);

        int fire_dx = 0, fire_dy = 0;
        do {
            fire_dx = rand() % 3 - 1;
            fire_dy = rand() % 3 - 1;
        } while (fire_dx == 0 && fire_dy == 0);
        
        fire(fire_dx, fire_dy);

        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

class RobotManager {
private:
    Battlefield battlefield;
    std::vector<std::shared_ptr<GenericRobot>> robots;
    std::queue<std::shared_ptr<GenericRobot>> respawnQueue;
    std::mt19937 rng{std::random_device{}()};

public:
    RobotManager(int width, int height) : battlefield(width, height) {}

    void addRobot(std::shared_ptr<GenericRobot> robot) {
        robots.push_back(robot);
        robot->setOnDestroyedCallback([this](GenericRobot* r) { robotDestroyed(r); });
        battlefield.placeRobot(robot->getName(), robot->getPosition().first, robot->getPosition().second);
    }

    void robotDestroyed(GenericRobot* robot) {
        robot->decrementLives();
        if (robot->getLives() > 0) {
            std::cout << robot->getName() << " will reenter the battlefield later." << std::endl;
            // Find the shared_ptr version of this robot
            for (auto& r : robots) {
                if (r.get() == robot) {
                    respawnQueue.push(r);
                    break;
                }
            }
        } else {
            std::cout << robot->getName() << " is permanently destroyed." << std::endl;
        }
    }

    void respawnOneRobot() {
        if (respawnQueue.empty()) return;

        auto robot = respawnQueue.front();
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

    void upgradeRobot(std::shared_ptr<GenericRobot> oldRobot, std::shared_ptr<GenericRobot> newRobot) {
        // Find and remove the old robot
        auto it = std::find_if(robots.begin(), robots.end(), 
            [&](const auto& r) { return r.get() == oldRobot.get(); });
        
        if (it != robots.end()) {
            // Remove from battlefield
            battlefield.removeRobot(oldRobot->getName());
            
            // Replace in vector
            *it = newRobot;
            
            // Add to battlefield
            battlefield.placeRobot(newRobot->getName(), 
                                newRobot->getPosition().first, 
                                newRobot->getPosition().second);
            
            // Set callback
            newRobot->setOnDestroyedCallback([this](GenericRobot* r) { robotDestroyed(r); });
        }
    }

    void takeTurn() {
        for (auto& robot : robots) {
            robot->takeTurn();
        }
        respawnOneRobot();
        battlefield.display();
    }

    Battlefield& getBattlefield() { return battlefield; }
};

#endif // ROBOT_H