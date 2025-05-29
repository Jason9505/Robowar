/**********|**********|**********|
Program: RobotUpgrades.h
Course: Data Structures and Algorithms
Trimester: 2410
Name: Jason Hean Qi Shen
ID: 242UC244FW
Lecture Section: TC3L
Tutorial Section: TT12L
Email: jason.hean.qi@studen.mmu.edu.my
Phone: 016-5556355
**********|**********|**********/
#ifndef ROBOTUPGRADES_H
#define ROBOTUPGRADES_H

#include "Robot.h"

// JumpBot - Can teleport to random positions on the battlefield
class JumpBot : public GenericRobot {
private:
    int jumpCount;          // Number of jumps remaining
    int battlefieldWidth;   // Cached battlefield dimensions
    int battlefieldHeight;

public:
    JumpBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf),
          jumpCount(3), battlefieldWidth(bf->getWidth()), 
          battlefieldHeight(bf->getHeight()) {
        upgraded = true;
        currentUpgradeType = 0;
        upgradeCount = 1;
    }

    // JumpBot turn sequence - jumps randomly then fires
    void takeTurn() override {
        think();
        // Calculate random position to jump to
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

        // Fire in a random direction (not at self)
        int fire_dx = 0, fire_dy = 0;
        do {
            fire_dx = rand() % 3 - 1;
            fire_dy = rand() % 3 - 1;
        } while (fire_dx == 0 && fire_dy == 0);
        fire(fire_dx, fire_dy);
    }
};

// SemiAutoBot - Fires multiple shots per turn with limited ammunition
class SemiAutoBot : public GenericRobot {
public:
    SemiAutoBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf) {
        setShells(15);  // Lower ammo count than basic robot
        upgraded = true;
        currentUpgradeType = 1;
        upgradeCount = 1;
    }

    // Fires three shots in the same direction if ammo is available
    void fire(int dx, int dy) override {
        if (getShells() <= 0) {
            std::cout << getName() << " has no shells left and cannot fire\n";
            return;
        }

        // Fire up to 3 times in the same direction
        for (int i = 0; i < 3 && getShells() > 0; i++) {
            GenericRobot::fire(dx, dy);
        }
    }
};

// TrackBot - Can track other robots and target them specifically
class TrackBot : public GenericRobot {
private:
    int trackersLeft;  // Number of robots that can still be tracked
    std::map<std::string, std::pair<int, int>> trackedRobots;  // Map of tracked robots

public:
    TrackBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf), trackersLeft(3) {
        upgraded = true;
        currentUpgradeType = 2;
        upgradeCount = 1;
    }

    // TrackBot turn sequence - tracks robots and fires at nearby tracked targets
    void takeTurn() override {
        think();

        // Attempt to track a new robot if tracking slots are available
        if (trackersLeft > 0) {
            const auto& positions = getBattlefield()->getRobotPositions();
            for (const auto& entry : positions) {
                // Don't track self or already-tracked robots
                if (entry.first != getName() && trackedRobots.find(entry.first) == trackedRobots.end()) {
                    trackedRobots[entry.first] = entry.second;
                    trackersLeft--;
                    std::cout << getName() << " is now tracking " << entry.first
                              << " at (" << entry.second.first << ", " << entry.second.second << ")\n";
                    break;
                }
            }
        }

        // Fire at any tracked robot that's adjacent
        for (const auto& entry : trackedRobots) {
            auto pos = entry.second;
            int dx = pos.first - getPosition().first;
            int dy = pos.second - getPosition().second;

            // Fire if target is adjacent (distance of 1 in any direction)
            if (abs(dx) <= 1 && abs(dy) <= 1) {
                fire(dx, dy);
                break;
            }
        }

        // Move randomly like basic robot
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

// Implementation of GenericRobot's upgrade function
inline std::shared_ptr<GenericRobot> GenericRobot::applyRandomUpgrade() {
    // Check upgrade limits
    if (upgraded && upgradeCount >= 2) {
        std::cout << name << " has already reached the maximum upgrade limit (2).\n";
        return nullptr;
    }

    // Determine available upgrade types
    std::vector<int> availableUpgrades = {0, 1, 2};
    
    // Remove current upgrade type if already upgraded
    if (upgraded) {
        availableUpgrades.erase(std::remove(availableUpgrades.begin(), 
                                 availableUpgrades.end(), currentUpgradeType), 
                                 availableUpgrades.end());
    }

    if (availableUpgrades.empty()) {
        std::cout << name << " has no available upgrade types left.\n";
        return nullptr;
    }

    // Select and apply random upgrade
    int choice = availableUpgrades[rand() % availableUpgrades.size()];
    currentUpgradeType = choice;
    upgradeCount++;

    std::cout << name << " is receiving upgrade #" << upgradeCount << "! ";

    // Create the appropriate upgraded robot
    std::shared_ptr<GenericRobot> upgradedRobot;
    switch (choice) {
        case 0:  // JumpBot upgrade
            std::cout << "Becoming a JumpBot!\n";
            upgradedRobot = std::make_shared<JumpBot>(name, x, y, battlefield);
            break;
        case 1:  // SemiAutoBot upgrade
            std::cout << "Becoming a SemiAutoBot!\n";
            upgradedRobot = std::make_shared<SemiAutoBot>(name, x, y, battlefield);
            break;
        case 2:  // TrackBot upgrade
            std::cout << "Becoming a TrackBot!\n";
            upgradedRobot = std::make_shared<TrackBot>(name, x, y, battlefield);
            break;
    }

    return upgradedRobot;
}

#endif // ROBOTUPGRADES_H