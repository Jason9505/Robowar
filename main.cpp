#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>

#include "Battlefield.h"
#include "ConfigLoader.h"
#include "GenericRobot.h"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    ConfigLoader loader("config.txt");  // use new file name
    if (!loader.load()) {
        std::cerr << "Failed to load config.txt" << std::endl;
        return 1;
    }

    Battlefield battlefield(loader.getWidth(), loader.getHeight());

    std::vector<std::shared_ptr<GenericRobot>> robots;

    for (const auto& robotData : loader.getRobots()) {
        if (battlefield.placeRobot(robotData.name, robotData.x, robotData.y)) {
            robots.push_back(std::make_shared<GenericRobot>(
                robotData.name, robotData.x, robotData.y, &battlefield
            ));
        } else {
            std::cerr << "Could not place robot " << robotData.name << " at ("
                      << robotData.x << ", " << robotData.y << ")\n";
        }
    }

    // Use dynamic number of steps from config
    int steps = loader.getSteps();
    for (int t = 0; t < steps; ++t) {
        std::cout << "\n--- Turn " << t + 1 << " ---\n";

        for (auto& robot : robots) {
            if (battlefield.getRobotPosition(robot->getName()).first == -1)
                continue;

            robot->takeTurn();
        }

        battlefield.display();
    }

    return 0;
}
