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
            int upgradeType = rand() % 3;  // Randomly select upgrade type
            switch (upgradeType) {
                case 0:  // JumpBot - can teleport around battlefield
                    robot = std::make_shared<JumpBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 1:  // SemiAutoBot - fires multiple shots per turn
                    robot = std::make_shared<SemiAutoBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 2:  // TrackBot - can track and target other robots
                    robot = std::make_shared<TrackBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
            }
        } else {
            // Create basic robot
            robot = std::make_shared<GenericRobot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
        }

        // Attempt to place robot on battlefield
        if (manager.addRobot(robot)) {
            robots.push_back(robot);
        } else {
            std::cerr << "Could not place robot " << robotData.name << " at ("
                      << robotData.x << ", " << robotData.y << ")\n";
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