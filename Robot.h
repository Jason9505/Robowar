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
#include <algorithm>
#include "Battlefield.h"

// Forward declarations for upgraded robots
class JumpBot;
class SemiAutoBot;
class TrackBot;

// Abstract base class
class Robot {
protected:
    std::string name;
    int x, y;
    Battlefield* battlefield;
public:
    Robot(const std::string& name, int x, int y, Battlefield* bf) 
        : name(name), x(x), y(y), battlefield(bf) {}
    virtual ~Robot() = default;
    
    std::string getName() const { return name; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
    Battlefield* getBattlefield() const { return battlefield; }
};

// Capability interfaces
class MovingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void move(int dx, int dy) = 0;
};

class ShootingRobot : virtual public Robot {
protected:
    int shells = 100;
public:
    using Robot::Robot;
    virtual void fire(int dx, int dy) = 0;
    int getShells() const { return shells; }
    void setShells(int s) { shells = s; }
};

class SeeingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void look(int dx, int dy) = 0;
};

class ThinkingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void think() = 0;
};

// Base concrete robot class
class GenericRobot : public MovingRobot, public ShootingRobot, 
                    public SeeingRobot, public ThinkingRobot {
protected:
    int lives = 3;
    bool upgraded = false;
    int upgradeCount = 0;
    int currentUpgradeType = -1;
    std::function<void(GenericRobot*)> onDestroyedCallback;
    
public:
    GenericRobot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf) {}
        
    // Implement pure virtual functions
    void move(int dx, int dy) override {
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

    void fire(int dx, int dy) override {
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

        if (killedSomeone && !upgraded && upgradeCount < 2) {
            auto upgradedRobot = applyRandomUpgrade();
            if (upgradedRobot) {
                if (onDestroyedCallback) {
                    onDestroyedCallback(this);
                }
            }
        }
    }

    void look(int dx, int dy) override {
        int lx = x + dx;
        int ly = y + dy;
        std::cout << name << " looks at (" << lx << ", " << ly << ")" << std::endl;
    }

    void think() override {
        std::cout << name << " is thinking..." << std::endl;
    }

    int getLives() const { return lives; }
    void decrementLives() { if (lives > 0) lives--; }
    bool hasUpgrade() const { return upgraded; }

    void setOnDestroyedCallback(std::function<void(GenericRobot*)> callback) {
        onDestroyedCallback = callback;
    }

    virtual std::shared_ptr<GenericRobot> applyRandomUpgrade();
    
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

    bool addRobot(std::shared_ptr<GenericRobot> robot) {
        bool placed = battlefield.placeRobot(robot->getName(), robot->getPosition().first, robot->getPosition().second);
        if (placed) {
            robots.push_back(robot);
            robot->setOnDestroyedCallback([this](GenericRobot* r) { robotDestroyed(r); });
        }
        return placed;
    }

    void robotDestroyed(GenericRobot* robot) {
        robot->decrementLives();
        if (robot->getLives() > 0) {
            std::cout << robot->getName() << " will reenter the battlefield later." << std::endl;
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
        auto it = std::find_if(robots.begin(), robots.end(), 
            [&](const auto& r) { return r.get() == oldRobot.get(); });
        
        if (it != robots.end()) {
            battlefield.removeRobot(oldRobot->getName());
            *it = newRobot;
            battlefield.placeRobot(newRobot->getName(), 
                                newRobot->getPosition().first, 
                                newRobot->getPosition().second);
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