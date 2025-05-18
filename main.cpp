#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>

#include "Battlefield.h"
#include "ConfigLoader.h"
#include "GenericRobot.h"
#include "GameManager.h"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    ConfigLoader loader("config.txt");
    if (!loader.load()) {
        std::cerr << "Failed to load config.txt" << std::endl;
        return 1;
    }

    GameManager manager(loader.getWidth(), loader.getHeight());

    std::vector<std::shared_ptr<GenericRobot>> robots;

    for (const auto& robotData : loader.getRobots()) {
        auto robot = std::make_shared<GenericRobot>(
            robotData.name, robotData.x, robotData.y, &manager.getBattlefield()
        );

        if (manager.getBattlefield().placeRobot(robotData.name, robotData.x, robotData.y)) {
            manager.addRobot(robot.get());
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