/**********|**********|**********|
Program: Robot.h 
Course: Data Structures and Algorithms
Trimester: 2410
Name: Jason Hean Qi Shen
ID: 242UC244FW
Lecture Section: TC3L
Tutorial Section: TT12L
Email: jason.hean.qi@studen.mmu.edu.my
Phone: 016-5556355
**********|**********|**********/
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

// Forward declarations for upgraded robot classes
class JumpBot;
class SemiAutoBot;
class TrackBot;
class KamikazeBot;
class SniperBot;
class MedicBot;

// Abstract base class for all robots
class Robot {
protected:
    std::string name;  // Robot identifier
    int x, y;          // Current position
    Battlefield* battlefield;  // Reference to the battlefield
public:
    Robot(const std::string& name, int x, int y, Battlefield* bf) 
        : name(name), x(x), y(y), battlefield(bf) {}
    virtual ~Robot() = default;
    
    // Accessors for robot properties
    std::string getName() const { return name; }
    std::pair<int, int> getPosition() const { return {x, y}; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
    Battlefield* getBattlefield() const { return battlefield; }
    virtual int getLives() const { return 0; }  // Base implementation
    virtual void heal() {}  // Base implementation
    virtual bool needsHealing() const { return false; }  // Base implementation
};

// Interface for robots that can move
class MovingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void move(int dx, int dy) = 0;  // Move by relative coordinates
};

// Interface for robots that can shoot
class ShootingRobot : virtual public Robot {
protected:
    int shells = 100;  // Ammunition count
public:
    using Robot::Robot;
    virtual void fire(int dx, int dy) = 0;  // Fire in relative direction
    int getShells() const { return shells; }
    void setShells(int s) { shells = s; }
};

// Interface for robots that can look around
class SeeingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void look(int dx, int dy) = 0;  // Look in relative direction
};

// Interface for robots that can think/process
class ThinkingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void think() = 0;  // Perform thinking/processing
};

// Base concrete robot class implementing all capabilities
class GenericRobot : public MovingRobot, public ShootingRobot, 
                    public SeeingRobot, public ThinkingRobot {
protected:
    int lives = 3;  // Number of respawns before permanent destruction
    bool upgraded = false;  // Whether robot has been upgraded
    int upgradeCount = 0;   // Number of upgrades applied
    int currentUpgradeType = -1;  // Type of current upgrade
    std::function<void(GenericRobot*)> onDestroyedCallback;  // Callback when destroyed
    
public:
    GenericRobot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf) {}
        
    // Implement movement capability
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

    // Implement shooting capability
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
        if ((rand() % 100) < 70) {  // 70% chance to hit
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

        // Check for upgrade eligibility after a kill
        if (killedSomeone && !upgraded && upgradeCount < 2) {
            auto upgradedRobot = applyRandomUpgrade();
            if (upgradedRobot) {
                if (onDestroyedCallback) {
                    onDestroyedCallback(this);
                }
            }
        }
    }

    // Implement looking capability
    void look(int dx, int dy) override {
        int lx = x + dx;
        int ly = y + dy;
        std::cout << name << " looks at (" << lx << ", " << ly << ")" << std::endl;
    }

    // Implement thinking capability
    void think() override {
        std::cout << name << " is thinking..." << std::endl;
    }

    // Accessors for robot state
    int getLives() const override { return lives; }
    void decrementLives() { if (lives > 0) lives--; }
    bool hasUpgrade() const { return upgraded; }

    // Set callback for when robot is destroyed
    void setOnDestroyedCallback(std::function<void(GenericRobot*)> callback) {
        onDestroyedCallback = callback;
    }

    // Applies a random upgrade to the robot (implemented in RobotUpgrade.h)
    virtual std::shared_ptr<GenericRobot> applyRandomUpgrade();
    
    // Default turn sequence for basic robots
    virtual void takeTurn() {
        think();
        
        // Randomly look in a direction
        int look_dx = rand() % 3 - 1;
        int look_dy = rand() % 3 - 1;
        look(look_dx, look_dy);

        // Fire in a random direction (not at self)
        int fire_dx = 0, fire_dy = 0;
        do {
            fire_dx = rand() % 3 - 1;
            fire_dy = rand() % 3 - 1;
        } while (fire_dx == 0 && fire_dy == 0);
        
        fire(fire_dx, fire_dy);

        // Move in a random direction
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }

    // Healing-related methods
    void heal() override { 
        if (lives < 3) {
            lives++;
            std::cout << getName() << " was healed! Now has " << lives << " lives.\n";
        }
    }
    bool needsHealing() const override { return lives < 3; }
};

// Manages all robots on the battlefield
class RobotManager {
private:
    Battlefield battlefield;  // The battlefield grid
    std::vector<std::shared_ptr<GenericRobot>> robots;  // All active robots
    std::queue<std::shared_ptr<GenericRobot>> respawnQueue;  // Robots waiting to respawn
    std::mt19937 rng{std::random_device{}()};  // Random number generator

public:
    RobotManager(int width, int height) : battlefield(width, height) {}

    // Adds a robot to the battlefield if position is valid
    bool addRobot(std::shared_ptr<GenericRobot> robot) {
        bool placed = battlefield.placeRobot(robot->getName(), robot->getPosition().first, robot->getPosition().second);
        if (placed) {
            robots.push_back(robot);
            battlefield.registerRobotReference(robot->getName(), robot);  // Register the reference
            robot->setOnDestroyedCallback([this](GenericRobot* r) { robotDestroyed(r); });
        }
        return placed;
    }

    // Handles robot destruction (called via callback)
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

    // Attempts to respawn one destroyed robot at a random empty position
    void respawnOneRobot() {
        if (respawnQueue.empty()) return;

        auto robot = respawnQueue.front();
        respawnQueue.pop();

        // Try up to 100 random positions for respawn
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

    // Replaces a robot with an upgraded version
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

    // Processes one turn of the simulation
    void takeTurn() {
        for (auto& robot : robots) {
            robot->takeTurn();  // Each robot takes its turn
        }
        respawnOneRobot();      // Attempt to respawn destroyed robots
        battlefield.display();  // Show the current battlefield state
    }

    // Accessor for the battlefield
    Battlefield& getBattlefield() { return battlefield; }
};

#endif // ROBOT_H