#ifndef ROBOTUPGRADES_H
#define ROBOTUPGRADES_H

#include "Robot.h"

// ------------------ JumpBot ------------------
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
        think();
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

        int fire_dx = 0, fire_dy = 0;
        do {
            fire_dx = rand() % 3 - 1;
            fire_dy = rand() % 3 - 1;
        } while (fire_dx == 0 && fire_dy == 0);
        fire(fire_dx, fire_dy);
    }
};

// ------------------ SemiAutoBot ------------------
class SemiAutoBot : public GenericRobot {
public:
    SemiAutoBot(const std::string& name, int x, int y, Battlefield* bf)
        : GenericRobot(name, x, y, bf) {
        setShells(15);
        upgraded = true;
    }

    void fire(int dx, int dy) override {
        if (getShells() <= 0) {
            std::cout << getName() << " has no shells left and cannot fire\n";
            return;
        }

        for (int i = 0; i < 3 && getShells() > 0; i++) {
            GenericRobot::fire(dx, dy);
        }
    }
};

// ------------------ TrackBot ------------------
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

        for (const auto& entry : trackedRobots) {
            auto pos = entry.second;
            int dx = pos.first - getPosition().first;
            int dy = pos.second - getPosition().second;

            if (abs(dx) <= 1 && abs(dy) <= 1) {
                fire(dx, dy);
                break;
            }
        }

        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

// ------------------ GenericRobot::applyRandomUpgrade ------------------
inline std::shared_ptr<GenericRobot> GenericRobot::applyRandomUpgrade() {
    if (upgraded && upgradeCount >= 2) {
        std::cout << name << " has already reached the maximum upgrade limit (2).\n";
        return nullptr;
    }

    // Create a vector of available upgrades (0: JumpBot, 1: SemiAutoBot, 2: TrackBot)
    std::vector<int> availableUpgrades = {0, 1, 2};
    
    // Remove already obtained upgrades from available options
    if (upgraded) {
        availableUpgrades.erase(std::remove(availableUpgrades.begin(), availableUpgrades.end(), currentUpgradeType), 
                               availableUpgrades.end());
    }

    if (availableUpgrades.empty()) {
        std::cout << name << " has no available upgrade types left.\n";
        return nullptr;
    }

    // Randomly select from remaining available upgrades
    int choice = availableUpgrades[rand() % availableUpgrades.size()];
    currentUpgradeType = choice; // Store the current upgrade type
    upgradeCount++; // Increment upgrade count

    std::cout << name << " is receiving upgrade #" << upgradeCount << "! ";

    std::shared_ptr<GenericRobot> upgradedRobot;
    switch (choice) {
        case 0:
            std::cout << "Becoming a JumpBot!\n";
            upgradedRobot = std::make_shared<JumpBot>(name, x, y, battlefield);
            break;
        case 1:
            std::cout << "Becoming a SemiAutoBot!\n";
            upgradedRobot = std::make_shared<SemiAutoBot>(name, x, y, battlefield);
            break;
        case 2:
            std::cout << "Becoming a TrackBot!\n";
            upgradedRobot = std::make_shared<TrackBot>(name, x, y, battlefield);
            break;
    }

    return upgradedRobot;
}

#endif // ROBOTUPGRADES_H
