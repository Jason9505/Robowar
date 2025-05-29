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
    srand(static_cast<unsigned>(time(nullptr)));  // Only place srand

    ConfigLoader loader("config.txt");
    if (!loader.load()) {
        std::cerr << "Failed to load config.txt" << std::endl;
        return 1;
    }

    RobotManager manager(loader.getWidth(), loader.getHeight());

    std::vector<std::shared_ptr<GenericRobot>> robots;

    for (const auto& robotData : loader.getRobots()) {
        std::shared_ptr<GenericRobot> robot;

        // Small chance to start with an upgraded robot
        if (rand() % 10 == 0) {
            int upgradeType = rand() % 3;
            switch (upgradeType) {
                case 0:
                    robot = std::make_shared<JumpBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 1:
                    robot = std::make_shared<SemiAutoBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
                case 2:
                    robot = std::make_shared<TrackBot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
                    break;
            }
        } else {
            robot = std::make_shared<GenericRobot>(robotData.name, robotData.x, robotData.y, &manager.getBattlefield());
        }

        if (manager.addRobot(robot)) {
            robots.push_back(robot);
        } else {
            std::cerr << "Could not place robot " << robotData.name << " at ("
                      << robotData.x << ", " << robotData.y << ")\n";
        }
    }

    int steps = loader.getSteps();
    for (int t = 0; t < steps; ++t) {
        std::cout << "\n--- Turn " << t + 1 << " ---\n";
        manager.takeTurn();
    }

    return 0;
}
