#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Battlefield.h"
#include "ConfigLoader.h"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // For random positions

    ConfigLoader loader("config.txt");
    if (!loader.load()) {
        std::cerr << "Failed to load configuration file." << std::endl;
        return 1;
    }

    Battlefield battlefield(loader.getWidth(), loader.getHeight());

    for (const auto& robot : loader.getRobots()) {
        if (!battlefield.placeRobot(robot.name, robot.x, robot.y)) {
            std::cerr << "Failed to place robot: " << robot.name << " at (" 
                      << robot.x << ", " << robot.y << ")" << std::endl;
        }
    }

    battlefield.display();
    return 0;
}
