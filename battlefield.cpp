#include <iostream>
#include "Battlefield.h"

int main() {
    Battlefield battlefield(20, 20);  // Create 20x20 battlefield

    // Place a few robots
    battlefield.placeRobot("Alpha", 2, 3);
    battlefield.placeRobot("Bravo", 5, 6);

    // Move a robot
    battlefield.moveRobot("Alpha", 3, 3);

    // Display battlefield
    battlefield.display();

    return 0;
}
