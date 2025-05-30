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

// KamikazeBot - Moves toward enemies and self-destructs when adjacent
class KamikazeBot : public GenericRobot {
private:
    bool hasDetonated = false;

public:
    KamikazeBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf) {
        upgraded = true;
        currentUpgradeType = 3;  // New upgrade type
        upgradeCount = 1;
    }

    void takeTurn() override {
        think();
        
        // Check for adjacent enemies
        bool enemyAdjacent = false;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int checkX = x + dx;
                int checkY = y + dy;
                
                if (battlefield->isInside(checkX, checkY) && 
                    battlefield->isOccupied(checkX, checkY)) {
                    enemyAdjacent = true;
                    break;
                }
            }
            if (enemyAdjacent) break;
        }
        
        if (enemyAdjacent && !hasDetonated) {
            // Self-destruct
            std::cout << name << " is self-destructing!\n";
            hasDetonated = true;
            
            // Damage all adjacent robots
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int checkX = x + dx;
                    int checkY = y + dy;
                    
                    if (battlefield->isInside(checkX, checkY)) {
                        const auto& positions = battlefield->getRobotPositions();
                        for (const auto& entry : positions) {
                            const auto& pos = entry.second;
                            if (pos.first == checkX && pos.second == checkY) {
                                std::cout << "Explosion hit " << entry.first << " at (" 
                                          << checkX << ", " << checkY << ")\n";
                                battlefield->removeRobot(entry.first);
                                break;
                            }
                        }
                    }
                }
            }
            
            // Destroy self
            battlefield->removeRobot(name);
            if (onDestroyedCallback) {
                onDestroyedCallback(this);
            }
        } else if (!hasDetonated) {
            // Move randomly toward enemies
            int move_dx = rand() % 3 - 1;
            int move_dy = rand() % 3 - 1;
            move(move_dx, move_dy);
        }
    }
};

// SniperBot - Has limited but highly accurate long-range shots
class SniperBot : public GenericRobot {
private:
    int remainingShells = 5; // Fewer but more powerful shots

public:
    SniperBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf) {
        upgraded = true;
        currentUpgradeType = 4;  // New upgrade type
        upgradeCount = 1;
        setShells(remainingShells); // Use the existing shells mechanism
    }

    void takeTurn() override {
        think();
        
        if (getShells() <= 0) return;
        
        // Look for enemies in long range (up to 5 squares away)
        for (int distance = 5; distance >= 1; distance--) {
            for (int dy = -distance; dy <= distance; ++dy) {
                for (int dx = -distance; dx <= distance; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    if (abs(dx) + abs(dy) != distance) continue; // Only check at current distance
                    
                    int checkX = x + dx;
                    int checkY = y + dy;
                    
                    if (battlefield->isInside(checkX, checkY) && 
                        battlefield->isOccupied(checkX, checkY)) {
                        // Found an enemy, take the shot
                        if (sniperFire(dx, dy)) {
                            return;
                        }
                    }
                }
            }
        }
        
        // If no target found at range, move randomly
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }

    bool sniperFire(int dx, int dy) {
        if (getShells() <= 0 || (dx == 0 && dy == 0)) return false;
        
        int targetX = x + dx;
        int targetY = y + dy;
        
        if (!battlefield->isInside(targetX, targetY)) return false;
        
        setShells(getShells() - 1);
        std::cout << getName() << " took a sniper shot at (" 
                  << targetX << "," << targetY << "). Shells left: " 
                  << getShells() << "\n";
        
        // Sniper shots have higher accuracy (90%)
        if ((rand() % 100) < 90) {
            const auto& positions = battlefield->getRobotPositions();
            for (const auto& entry : positions) {
                const auto& pos = entry.second;
                if (pos.first == targetX && pos.second == targetY) {
                    std::cout << "Direct hit! " << entry.first << " at (" 
                              << targetX << ", " << targetY << ") was destroyed!\n";
                    battlefield->removeRobot(entry.first);
                    return true;
                }
            }
        } else {
            std::cout << "The sniper shot missed!\n";
        }
        
        return true;
    }
};

class MedicBot : public GenericRobot {
private:
    int healCharges = 3;

public:
    MedicBot(const std::string& name, int x, int y, Battlefield* bf)
        : Robot(name, x, y, bf), GenericRobot(name, x, y, bf) {
        upgraded = true;
        currentUpgradeType = 5;
        upgradeCount = 1;
    }

    void takeTurn() override {
        think();
        
        // First try to heal adjacent allies
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int checkX = x + dx;
                int checkY = y + dy;
                
                if (battlefield->isInside(checkX, checkY) && healCharges > 0) {
                    const auto& positions = battlefield->getRobotPositions();
                    for (const auto& entry : positions) {
                        const auto& pos = entry.second;
                        if (pos.first == checkX && pos.second == checkY && entry.first != name) {
                            auto robotToHeal = battlefield->getRobotReference(entry.first);
                            if (robotToHeal && robotToHeal->needsHealing()) {
                                robotToHeal->heal();
                                healCharges--;
                                std::cout << name << " healed " << entry.first 
                                          << " at (" << checkX << ", " << checkY 
                                          << "). Heals left: " << healCharges << "\n";
                                return;
                            }
                        }
                    }
                }
            }
        }
        
        // If no healing done, move randomly
        int move_dx = rand() % 3 - 1;
        int move_dy = rand() % 3 - 1;
        move(move_dx, move_dy);
    }
};

inline std::shared_ptr<GenericRobot> GenericRobot::applyRandomUpgrade() {
    // Check upgrade limits
    if (upgraded && upgradeCount >= 2) {
        std::cout << name << " has already reached the maximum upgrade limit (2).\n";
        return nullptr;
    }

    // Determine available upgrade types (now 0-5)
    std::vector<int> availableUpgrades = {0, 1, 2, 3, 4, 5};
    
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
        case 3:  // KamikazeBot upgrade
            std::cout << "Becoming a KamikazeBot!\n";
            upgradedRobot = std::make_shared<KamikazeBot>(name, x, y, battlefield);
            break;
        case 4:  // SniperBot upgrade
            std::cout << "Becoming a SniperBot!\n";
            upgradedRobot = std::make_shared<SniperBot>(name, x, y, battlefield);
            break;
        case 5:  // MedicBot upgrade
            std::cout << "Becoming a MedicBot!\n";
            upgradedRobot = std::make_shared<MedicBot>(name, x, y, battlefield);
            break;
    }

    return upgradedRobot;
}

#endif // ROBOTUPGRADES_H