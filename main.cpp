/**********|**********|**********|
Program: main.cpp
Course: Data Structures and Algorithms
Trimester: 2410
Name: Jason Hean Qi Shen
ID: 242UC244FW
Lecture Section: TC3L
Tutorial Section: TT12L
Email: jason.hean.qi@studen.mmu.edu.my
Phone: 016-5556355
**********|**********|**********/
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>

#include "Battlefield.h"
#include "ConfigLoader.h"
#include "Robot.h"
#include "RobotUpgrade.h"

int main() {
    // Initialize random number generator for the entire program
    srand(static_cast<unsigned>(time(nullptr)));

    // Load configuration from file
    ConfigLoader loader("config.txt");
    if (!loader.load()) {
        std::cerr << "Failed to load config.txt" << std::endl;
        return 1;
    }

    // Create battlefield manager with configured dimensions
    RobotManager manager(loader.getWidth(), loader.getHeight());

    // Container for all robot instances
    std::vector<std::shared_ptr<GenericRobot>> robots;

    // Create and place robots based on configuration
    for (const auto& robotData : loader.getRobots()) {
        std::shared_ptr<GenericRobot> robot;

        // 10% chance to create an upgraded robot instead of a basic one
        if (rand() % 10 == 0) {
            int upgradeType = rand() % 6;  // Now 6 possible upgrade types (0-5)
            switch (upgradeType) {
                case 0:  // JumpBot
                    robot = std::make_shared<JumpBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 1:  // SemiAutoBot
                    robot = std::make_shared<SemiAutoBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 2:  // TrackBot
                    robot = std::make_shared<TrackBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 3:  // KamikazeBot
                    robot = std::make_shared<KamikazeBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 4:  // SniperBot
                    robot = std::make_shared<SniperBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 5:  // MedicBot (new)
                    robot = std::make_shared<MedicBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
            }
        }
            else {
                // Create a basic GenericRobot if no upgrade is applied
                robot = std::make_shared<GenericRobot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
            }

        // Attempt to place robot on battlefield
        if (manager.addRobot(robot)) {
            robots.push_back(robot);
        } else {
            std::cerr << "Could not place robot " << robotData.name << " at (" << robotData.x << ", " << robotData.y << ")\n";
        }
    }

    // Run simulation for configured number of steps
    int steps = loader.getSteps();
    for (int t = 0; t < steps; ++t) {
        std::cout << "\n--- Turn " << t + 1 << " ---\n";
        manager.takeTurn();  // Process each robot's turn
    }

    return 0;
}